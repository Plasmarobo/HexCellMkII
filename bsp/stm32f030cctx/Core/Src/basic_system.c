#include "basic_system.h"

#include "comm_phy.h"
#include "communication.h"
#include "crc.h"
#include "memory_layout.h"
#include "report.h"
#include "reset.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash.h"
#include "stm32f0xx_hal_flash_ex.h"

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

// Should be baked into image metadata and unchanged
#define HW_VERSION (0x01)
// 2KB for the STM32F030CCT6
#define PAGE_SIZE (2048)

#define REQUEST_RESET (1)
#define SKIP_RESET (0)

typedef enum
{
  FWU_IDLE = 0,
  FWU_ERASE_IMAGE,
  FWU_STREAM_IMAGE,
  FWU_PROGRAM_IMAGE,
  FWU_VERIFY_IMAGE,
  FWU_ERASE_METADATA,
  FWU_WRITE_METADATA,
} firmware_update_state_t;

// For the STM32F030CCT, we have a 2k page of metadata for the bootloader and app
static firmware_update_metadata_t incoming_fw_update;
static firmware_update_state_t    fwu_state;
static opt_callback_t             operation_callback;
static uint64_t*                  flash_write_ptr;
static uint32_t                   flash_address;
static int32_t                    flash_write_count;
static uint32_t                   flash_offset;
static uint16_t                   flash_crc;
static FLASH_EraseInitTypeDef     erase_data;
static comm_port_t                reply_port;
static message_t                  msg_buffer;
static message_header_t           busy_reply_buffer;
static uint8_t                    reset_reason;
static uint8_t                    reset_mode;

static void handle_error(uint32_t error_code)
{
  report_error(RT_ERROR, error_code, fwu_state, 0);
  switch (fwu_state)
  {
    case FWU_STREAM_IMAGE:
    case FWU_ERASE_METADATA:
    case FWU_WRITE_METADATA:
    case FWU_ERASE_IMAGE:
      // We may have just erased flash...
      HAL_FLASH_Lock();
      break;
    case FWU_VERIFY_IMAGE:
    case FWU_IDLE:
    default:
      break;
  }
  fwu_state                = FWU_IDLE;
  msg_buffer.header.status = error_code;
  msg_buffer.header.length = 0;
  queue_message_send(reply_port, &msg_buffer, NULL);
}

static void handle_reset(int32_t status, uint32_t user)
{
  hard_reset(reset_reason, reset_mode);
}

static void handle_metadata_update(int32_t status, uint32_t user)
{
  // Acknowledge the update and reset
  msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE_COMPLETE;
  msg_buffer.header.length = 0;
  opt_callback_t cb        = (reset_reason != RESET_REASON_INVALID) ? handle_reset : NULL;
  send(reply_port, &msg_buffer, cb);
}

// Fetch the capabilities of this board
firmware_update_metadata_t get_capability(uint32_t magic_number)
{
  firmware_update_metadata_t capability;
  const image_metadata_t*    img_meta = get_image_metadata(magic_number);
  if (NULL != img_meta)
  {
    capability.image_metadata = *img_meta;
  }
  else
  {
    capability.image_metadata.image_magic       = IMAGE_MAGIC_INVALID;
    capability.image_metadata.version_major     = 0;
    capability.image_metadata.version_minor     = 0;
    capability.image_metadata.version_patch     = 0;
    capability.image_metadata.hardware_revision = HW_VERSION;
    capability.image_metadata.hardware_id       = HARDWARE_ADDRESS_INVALID;
    capability.image_metadata.boot_address      = (uint32_t)_ORIGIN_BL;
  }
  // Align this to the next-smallest multiple of 8 bytes
  capability.chunk_size = COMM_BUFFER_LENGTH - sizeof(message_header_t);
  capability.chunk_size -= capability.chunk_size % sizeof(uint64_t);
  switch (magic_number)
  {
    case IMAGE_MAGIC_BL:
      capability.image_size = (uint32_t)_LENGTH_BL;
      break;
    case IMAGE_MAGIC_APP:
      capability.image_size = (uint32_t)_LENGTH_APP;
      break;
    case IMAGE_MAGIC_META:
      capability.image_size = (uint32_t)_LENGTH_META;
      break;
    default:
      capability.image_size = 0;
      break;
  }
  capability.crc   = 0;
  capability.flags = FWU_FLAG_FORCED | FWU_FLAG_SKIP_CRC | FWU_FLAG_VERIFY;
  return capability;
}

// Define the root: must be called in image MAIN before any image metadata functions are accessed
// We don't care WHAT we're updating, only where it's stored
static void image_matadata_init(uintptr_t root)
{
  current_metadata                              = (image_metadata_t*)root;
  fwu_state                                     = FWU_IDLE;
  operation_callback                            = NULL;
  incoming_fw_update.image_metadata.image_magic = IMAGE_MAGIC_EMPTY;
  busy_reply_buffer.status                      = BASIC_SYSTEM_MESSAGE_FW_UPDATE_BUSY;
  busy_reply_buffer.length                      = 0;
}

// Image metadata
const image_metadata_t* get_image_metadata(uint32_t image_type)
{
  const image_metadata_t* metadata = NULL;
  switch (image_type)
  {
    case IMAGE_MAGIC_BL:
      metadata = (const image_metadata_t*)_ORIGIN_BL_META;
      break;
    case IMAGE_MAGIC_META:
      // Can't get metadata data section: it doesn't exist
      break;
    case IMAGE_MAGIC_APP: // Intentional fallthrough
    default:
      metadata = (const image_metadata_t*)_ORIGIN_APP_META;
      break;
  }
  return metadata;
}

static uint32_t get_meta_address(uint32_t image_magic)
{
  uint32_t page_address = INVALID_ADDRESS;
  switch (image_magic)
  {
    case IMAGE_MAGIC_BL:
      page_address = _ORIGIN_BL_META;
      break;
    case IMAGE_MAGIC_APP:
      page_address = _ORIGIN_APP_META;
      break;
    default:
      break;
  }
  return page_address;
}

/**
 * @brief Set the image metadata object
 *
 * @param data When setting data, if the hardware id is preserved or invalid
 * it will be overwritten with the current id set in FLASH
 * If the hardware revision is invalid, it will be overwritten as well
 * @param cb Called on completion of the erase-program operation
 */
void set_image_metadata(image_metadata_t data, opt_callback_t cb)
{
  if ((FWU_IDLE == fwu_state) || (FWU_VERIFY_IMAGE == fwu_state))
  {
    // TODO: Verify image magic
    // Cache out current metadata: we most likely want to save the HW id
    const image_metadata_t* metadata_cache = get_image_metadata(data.image_magic);
    incoming_fw_update.image_metadata      = data;
    if ((HARDWARE_ADDRESS_INVALID == data.hardware_id) || (HARDWARE_ADDRESS_RESERVED == data.hardware_id))
    {
      incoming_fw_update.image_metadata.hardware_id = metadata_cache->hardware_id;
    }

    if (HARDWARE_REVISION_INVALID == data.hardware_revision)
    {
      incoming_fw_update.image_metadata.hardware_revision = metadata_cache->hardware_revision;
    }
    // Erasing metadata
    HAL_FLASH_Unlock();

    erase_data.PageAddress = get_meta_address(data.image_magic);
    erase_data.NbPages     = _LENGTH_META / PAGE_SIZE;
    erase_data.TypeErase   = FLASH_TYPEERASE_PAGES;
    fwu_state              = FWU_ERASE_METADATA;
    operation_callback     = cb;
    HAL_FLASHEx_Erase_IT(&erase_data);
  }
}

// FW update init
static void firmware_update_init(void)
{
  fwu_state          = FWU_IDLE;
  memset(&incoming_fw_update, 0, sizeof(firmware_update_metadata_t));
  operation_callback = NULL;
  flash_write_ptr    = NULL;
  flash_address      = 0;
  flash_write_count  = 0;
  flash_crc          = 0;
  reply_port         = PORT_MAX;
  reset_reason       = RESET_REASON_INVALID;
  reset_mode         = BOOT_MODE_APP;
}

static void firmware_update_handle_state(message_t* host_message)
{
  switch (fwu_state)
  {
    case FWU_IDLE:
      if (NULL != host_message)
      {
        if (BASIC_SYSTEM_MESSAGE_FW_UPDATE_START != host_message->header.status)
        {
          // We've received a command to continue a FW update that hasn't started...
          handle_error(FWU_ERROR_NOT_STARTED);
        }
        else if (sizeof(firmware_update_metadata_t) < host_message->header.length)
        {
          // Must contain enough data to fill the data structure
          handle_error(FWU_ERROR_INVALID_MESSAGE_LENGTH);
        }
        else
        {
          memcpy(&incoming_fw_update, host_message->data, sizeof(firmware_update_metadata_t));
          firmware_update_metadata_t capability = get_capability(incoming_fw_update.image_metadata.image_magic);
          if (incoming_fw_update.image_metadata.image_magic != capability.image_metadata.image_magic)
          {
            handle_error(FWU_ERROR_INVALID_IMAGE);
          }
          else if (incoming_fw_update.chunk_size > capability.chunk_size)
          {
            handle_error(FWU_ERROR_INVALID_CHUNK_SIZE);
          }
          else if (incoming_fw_update.image_size > capability.image_size)
          {
            handle_error(FWU_ERROR_INVALID_IMAGE_SIZE);
          }
          else if (current_metadata->image_magic == incoming_fw_update.image_metadata.image_magic)
          {
            // App can update the Bootloader, bootloader can update the app
            // Ensure we're in the correct image for updating our target
            reset_reason = RESET_REASON_FIRMWARE_UPDATE_START;
            if (IMAGE_MAGIC_APP == incoming_fw_update.image_metadata.image_magic)
            {
              reset_mode = BOOT_MODE_BOOTLOADER;
            }
            else if (IMAGE_MAGIC_BL == incoming_fw_update.image_metadata.image_magic)
            {
              reset_mode = BOOT_MODE_APP;
              // Check to see if we have an app we can boot into
              const image_metadata_t* app_data = get_image_metadata(IMAGE_MAGIC_APP);
              // Read out the image magic (alternatively could use boot address)
              // and check that it's valid. Abort if the app doesn't exist
              if ((NULL != app_data) && (IMAGE_MAGIC_APP == app_data->image_magic))
              {
                handle_error(FWU_ERROR_NO_APP);
                break;
              }
            }
            msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE_REBOOTING;
            msg_buffer.header.length = 0;
            queue_message_send(reply_port, &msg_buffer, handle_reset);
          }
          else
          {
            reset_reason                          = RESET_REASON_FIRMWARE_UPDATE_COMPLETE;
            fwu_state                             = FWU_ERASE_IMAGE;
            flash_offset                          = 0;
            msg_buffer.header.status              = BASIC_SYSTEM_MESSAGE_FW_UPDATE_START;
            msg_buffer.header.length              = sizeof(firmware_update_metadata_t);
            msg_buffer.header.destination_address = BROADCAST_ADDRESS;
            memcpy(msg_buffer.data, (uint8_t*)&capability, sizeof(firmware_update_metadata_t));
            queue_message_send(reply_port, &msg_buffer, NULL);
            // Erase Image
            switch (incoming_fw_update.image_metadata.image_magic)
            {
              case IMAGE_MAGIC_APP:
                flash_address      = _ORIGIN_APP;
                erase_data.NbPages = _LENGTH_APP / PAGE_SIZE;
                break;
              case IMAGE_MAGIC_BL:
                flash_address      = _ORIGIN_BL;
                erase_data.NbPages = _LENGTH_BL / PAGE_SIZE;
                break;
              case IMAGE_MAGIC_META:    // Intentional fallthrough
              case IMAGE_MAGIC_EMPTY:   // Intentional fallthrough
              case IMAGE_MAGIC_INVALID: // Intentional fallthrough
              default:
                handle_error(FWU_ERROR_INVALID_IMAGE);
                break;
            }
            HAL_FLASH_Unlock();
            fwu_state              = FWU_ERASE_IMAGE;
            erase_data.TypeErase   = FLASH_TYPEERASE_PAGES;
            erase_data.PageAddress = flash_address;
            HAL_FLASHEx_Erase_IT(&erase_data);
          }
        }
      }
      break;
    case FWU_STREAM_IMAGE:
      if ((NULL != host_message) && (host_message->header.status == BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA))
      {
        if (0 == flash_offset)
        {
          // Chunk must be 32 bit aligned (4 byte alignment)
          start_crc(host_message->data, host_message->header.length);
        }
        else
        {
          continue_crc(host_message->data, host_message->header.length);
        }
        // Write chunk out to flash
        memcpy(&msg_buffer, host_message, sizeof(message_t));
        flash_write_ptr = (uint64_t*)msg_buffer.data;
        // Should be aligned/padded to 8 bytes
        flash_write_count = msg_buffer.header.length;
        fwu_state         = FWU_PROGRAM_IMAGE;
        HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address + flash_offset, *flash_write_ptr);
      }
      break;
    case FWU_VERIFY_IMAGE:
      switch (incoming_fw_update.image_metadata.image_magic)
      {
        case IMAGE_MAGIC_APP:
          flash_address      = _ORIGIN_APP;
          erase_data.NbPages = _LENGTH_APP / PAGE_SIZE;
          break;
        case IMAGE_MAGIC_BL:
          flash_address      = _ORIGIN_BL;
          erase_data.NbPages = _LENGTH_BL / PAGE_SIZE;
          break;
        case IMAGE_MAGIC_META:    // Intentional fallthrough
        case IMAGE_MAGIC_EMPTY:   // Intentional fallthrough
        case IMAGE_MAGIC_INVALID: // Intentional fallthrough
        default:
          handle_error(FWU_ERROR_INVALID_IMAGE);
          break;
      }
      start_crc((uint8_t*)flash_address, incoming_fw_update.image_size);
      if (incoming_fw_update.crc != finish_crc())
      {
        handle_error(FWU_ERROR_CRC_MISMATCH);
      }
      else
      {
        set_image_metadata(incoming_fw_update.image_metadata, handle_metadata_update);
      }
      break;
    case FWU_ERASE_METADATA:
    case FWU_WRITE_METADATA:
    default:
      busy_reply_buffer.length = 0; // Critical that length is zero if we're casting to a message_t
      send(reply_port, (message_t*)&busy_reply_buffer, NULL);
      break;
  }
}

void basic_system_init(uintptr_t image_root)
{
  image_matadata_init(image_root);
  firmware_update_init();
}

// FW update update
void basic_system_handle_message(receive_opt_t incoming_message)
{
  // We only handle basic system messages
  switch (incoming_message.msg.header.status)
  {
    case BASIC_SYSTEM_MESSAGE_READ_IMAGE_METADATA:
      if (incoming_message.msg.header.length >= sizeof(uint32_t))
      {
        const image_metadata_t* img = get_image_metadata(*((uint32_t*)incoming_message.msg.data));
        memcpy(msg_buffer.data, img, sizeof(image_metadata_t));
        msg_buffer.header.length = sizeof(image_metadata_t);
        msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_READ_IMAGE_METADATA;
        send(incoming_message.origin_port, &msg_buffer, NULL);
      }
      else
      {
        handle_error(FWU_ERROR_INVALID_MESSAGE_LENGTH);
      }
      break;
    case BASIC_SYSTEM_MESSAGE_WRITE_IMAGE_METADATA:
      if (incoming_message.msg.header.length >= sizeof(image_metadata_t))
      {
        set_image_metadata(*((image_metadata_t*)incoming_message.msg.data), handle_metadata_update);
      }
      else
      {
        handle_error(FWU_ERROR_INVALID_MESSAGE_LENGTH);
      }
      break;
    case BASIC_SYSTEM_MESSAGE_RESET:
      msg_buffer.header.status = FWU_ERROR_NONE;
      msg_buffer.header.length = 0;
      send(incoming_message.origin_port, &msg_buffer, handle_reset);
      break;
    case BASIC_SYSTEM_MESSAGE_FW_UPDATE_START: // Intentional fallthrough
    case BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA:
      // If we already have a port, make sure it matches
      if ((PORT_MAX == reply_port) || (reply_port == incoming_message.origin_port))
      {
        reply_port = incoming_message.origin_port;
        firmware_update_handle_state(&incoming_message.msg);
      }
      break;
    default:
      handle_error(FWU_ERROR_UNKNOWN_REQUEST);
      break;
  }
}

void basic_system_update(opt_callback_t cb)
{
  // Handle any scheduled tasks outside interrupts
  if (FWU_VERIFY_IMAGE == fwu_state)
  {
    firmware_update_handle_state(NULL);
  }
  if (NULL != cb)
  {
    cb(0, 0);
  }
}

uint32_t get_hardware_id(void)
{
  uint32_t hwid = 0;
  if (NULL != current_metadata)
  {
    hwid = current_metadata->hardware_id;
  }
  return hwid;
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  if (HAL_FLASH_ERROR_NONE == ReturnValue)
  {
    switch (fwu_state)
    {
      case FWU_ERASE_IMAGE:
        fwu_state = FWU_STREAM_IMAGE;
        // Request data chunk, send index of chunk we are requesting (0 here)
        msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA;
        msg_buffer.header.length = sizeof(uint32_t);
        flash_offset             = 0;
        memcpy(msg_buffer.data, (uint8_t*)&flash_offset, sizeof(uint32_t));
        send(reply_port, &msg_buffer, NULL);
        break;
      case FWU_ERASE_METADATA: // FWU
        if ((incoming_fw_update.image_metadata.image_magic == IMAGE_MAGIC_APP) ||
            (incoming_fw_update.image_metadata.image_magic == IMAGE_MAGIC_BL))
        {
          // We have something to write so write it
          fwu_state         = FWU_WRITE_METADATA;
          flash_write_ptr   = (uint64_t*)&incoming_fw_update.image_metadata;
          flash_address     = get_meta_address(incoming_fw_update.image_metadata.image_magic);
          flash_offset      = 0;
          flash_write_count = sizeof(image_metadata_t);
          HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, *flash_write_ptr);
        }
        else
        {
          handle_error(FWU_ERROR_INVALID_IMAGE);
          fwu_state = FWU_IDLE;
          HAL_FLASH_Lock();
        }
        break;
      case FWU_WRITE_METADATA:
        flash_write_count -= sizeof(uint64_t);
        flash_offset += sizeof(uint64_t);
        if (flash_write_count > 0)
        {
          HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address + flash_offset, *flash_write_ptr);
        }
        else
        {
          HAL_FLASH_Lock();
          if (NULL != operation_callback)
          {
            operation_callback(ReturnValue, 0);
          }
        }
        break;
      case FWU_PROGRAM_IMAGE:
        flash_write_count -= sizeof(uint64_t);
        flash_offset += sizeof(uint64_t);
        if (flash_write_count > 0)
        {
          HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address + flash_offset, *flash_write_ptr);
        }
        else
        {
          if (incoming_fw_update.image_size > flash_offset)
          {
            // Ask for next chunk
            fwu_state                = FWU_STREAM_IMAGE;
            msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA;
            msg_buffer.header.length = sizeof(uint32_t);
            memcpy(msg_buffer.data, &flash_offset, sizeof(uint32_t));
            send(reply_port, &msg_buffer, NULL);
          }
          else
          {
            HAL_FLASH_Lock();
            // Notify host we're verifying
            fwu_state                = FWU_VERIFY_IMAGE;
            msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE_VERIFING;
            msg_buffer.header.length = 0;
            send(reply_port, &msg_buffer, NULL);
            if (NULL != operation_callback)
            {
              operation_callback(ReturnValue, 0);
            }
          }
        }
        break;
      default:
        break;
    }
  }
  else
  {
    // Write down the magic number
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, get_meta_address(incoming_fw_update.image_metadata.image_magic), 0);
    handle_error(FWU_ERROR_ERASE_FAILURE);
    if (NULL != operation_callback)
    {
      operation_callback(FWU_ERROR_ERASE_FAILURE, 0);
    }
    HAL_FLASH_Lock();
    fwu_state = FWU_IDLE;
  }
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, get_meta_address(incoming_fw_update.image_metadata.image_magic), 0);
  handle_error(FWU_ERROR_PROGRAM_FAILURE);
  if (NULL != operation_callback)
  {
    operation_callback(FWU_ERROR_PROGRAM_FAILURE, 0);
  }
  HAL_FLASH_Lock();
  fwu_state = FWU_IDLE;
}