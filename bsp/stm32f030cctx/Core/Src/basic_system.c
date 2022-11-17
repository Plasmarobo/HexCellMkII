#include "basic_system.h"

#include "comm_phy.h"

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash.h"
#include "stm32f0xx_hal_flash_ex.h"

#include <stdarg.h>

// Should be baked into image metadata and unchanged
#define HW_VERSION (0x01)
// 2KB for the STM32F030CCT6
#define PAGE_SIZE (2048)

typedef enum
{
  FWU_IDLE = 0,
  FWU_CLIENT_QUERY,
  FWU_ERASE_IMAGE,
  FWU_STREAM_IMAGE,
  FWU_VERIFY_IMAGE,
  FWU_ERASE_METADATA,
  FWU_WRITE_METADATA,
} firmware_update_state_t;

// For the STM32F030CCT, we have a 2k page of metadata for the bootloader and app
static uintptr_t                  image_root;
static firmware_update_metadata_t incoming_fw_update;
static firmware_update_state_t    fwu_state;
static opt_callback_t             operation_callback;
static uint64_t*                  flash_write_ptr;
static uint32_t                   flash_address;
static int32_t                    flash_write_count;
static uint16_t                   flash_crc;
static FLASH_EraseInitTypeDef     erase_data;
static comm_port_t                reply_port;
static message_t                  msg_buffer;

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
    capability.image_metadata.boot_address      = (uint32_t)_ORIGIN_BL;
  }
  capability.chunk_size = COMM_BUFFER_LENGTH;
  switch (magic_number)
  {
    case IMAGE_MAGIC_BL:
      capability.chunk_count = ((uint32_t)_LENGTH_BL) / capability.chunk_size;
      break;
    case IMAGE_MAGIC_APP:
      capability.chunk_count = ((uint32_t)_LENGTH_APP) / capability.chunk_size;
      break;
    case IMAGE_MAGIC_META:
      capability.chunk_count = ((uint32_t)_LENGTH_META) / capability.chunk_size;
      break;
    default:
      capability.chunk_count = 0;
      break;
  }
  capability.crc   = 0;
  capability.flags = FWU_FLAG_FORCED | FWU_FLAG_SKIP_CRC | FWU_FLAG_VERIFY;
  return capability;
}

// Define the root: must be called in image MAIN before any image metadata functions are accessed
// We don't care WHAT we're updating, only where it's stored
void image_matadata_init(uintptr_t root)
{
  image_root                                    = root;
  fwu_state                                     = FWU_IDLE;
  operation_callback                            = NULL;
  incoming_fw_update.image_metadata.image_magic = IMAGE_MAGIC_EMPTY;
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

uint32_t get_image_address(uint32_t image_magic)
{
  uint32_t page_address;
  switch (image_magic)
  {
    case IMAGE_MAGIC_BL:
      page_address = _ORIGIN_BL;
      break;
    case IMAGE_MAGIC_APP:
      page_address = _ORIGIN_APP;
      break;
    default:
      break;
  }
  return page_address;
}

uint32_t get_meta_address(uint32_t image_magic)
{
  uint32_t page_address;
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

void set_image_metadata(image_metadata_t data, opt_callback_t cb)
{
  if ((FWU_IDLE == fwu_state) || (FWU_VERIFY_IMAGE == fwu_state))
  {
    // Erasing metadata
    HAL_FLASH_Unlock(void);

    erase_data.PageAddress = get_meta_address(data.image_magic);
    erase_data.NbPages     = _LENGTH_META / PAGE_SIZE;
    erase_data.TypeErase   = FLASH_TYPEERASE_PAGES;
    fwu_state              = FWU_ERASE_METADATA;
    operation_callback     = cb;
    HAL_FLASHEx_Erase_IT(&erase_data);
  }
}

// FW update init
void firmware_update_init(void)
{
  fwu_state          = FWU_IDLE;
  image_root         = 0;
  incoming_fw_update = { 0 };
  operation_callback = NULL;
  flash_write_ptr    = NULL;
  flash_address      = 0;
  flash_write_count  = 0;
  flash_crc          = 0;
  reply_port         = PORT_MAX;
}

void firmware_update_start(firmware_update_metadata_t data, comm_port_t host_port)
{
  // Check the metadata
  uint32_t max_image_size;
  uint32_t error_code = FWU_ERROR_NONE;
  uint32_t address    = 0;
  uint32_t length     = 0;
  reply_port          = host_port;
  switch (data.image_metadata.image_magic)
  {
    case IMAGE_MAGIC_APP:
      max_image_size = _LENGTH_APP;
      address        = _ORIGIN_APP;
      length         = _LENGTH_APP;
      break;
    case IMAGE_MAGIC_BL:
      max_image_size = _LENGTH_BL;
      address        = _ORIGIN_BL;
      length         = _LENGTH_BL;
      break;
    case IMAGE_MAGIC_META:    // Intentional fallthrough
    case IMAGE_MAGIC_EMPTY:   // Intentional fallthrough
    case IMAGE_MAGIC_INVALID: // Intentional fallthrough
    default:
      error_code = FWU_ERROR_INVALID_IMAGE;
      break;
  }

  if (FWU_ERROR_NONE != error_code)
  {
    handle_error(error_code);
    return;
  }

  if (data.chunk_size > COMM_BUFFER_LENGTH)
  {
    // Error: chunk size too long
    handle_error(FWU_ERROR_CHUNK_TOO_LONG);
    return;
  }

  flash_write_count = (data.chunk_count * data.chunk_size);
  if (flash_write_count > max_image_size)
  {
    // Error: image wont fit
    handle_error(FWU_ERROR_IMAGE_TOO_LONG);
    return;
  }

  incoming_fw_update     = data;
  flash_address          = address;
  flash_write_ptr        = NULL; // Chunk reception will popluate
  fwu_state              = FWU_ERASE_IMAGE;
  erase_data.TypeErase   = FLASH_TYPEERASE_PAGES;
  erase_data.PageAddress = address;
  erase_data.NbPages     = length / PAGE_SIZE;
  HAL_FLASHEx_Erase_IT(&erase_data);
}

// FW update update
void firmware_update_handle_event(message_t incoming_message)
{
  if ((incoming_message.header.status < BASIC_SYSTEM_MESSAGE_TYPE) ||
      (incoming_message.header.status >= BASIC_SYSTEM_MESSAGE_MAX))
  {
    switch (fwu_state)
    {
      case FWU_IDLE:
        // A firmware update is started by querying the client (us)
        if (BASIC_SYSTEM_MESSAGE_FW_UPDATE_QUERY_CLIENT != incoming_message.header.status)
        {
          send_error(FWU_ERROR_INVALID_QUERY);
        }
        else if (sizeof(uint32_t) < incoming_message.header.length)
        {
          send_error(FWU_ERROR_INVALID_MESSAGE_LENGTH);
        }
        else
        {
          uint32_t image_magic;
          memcpy((uint8_t*)&image_magic, incoming_message.data, sizeof(uint32_t));
          fwu_state                             = FWU_CLIENT_QUERY;
          msg_buffer.header.status              = BASIC_SYSTEM_MESSAGE_FW_UPDATE_QUERY_CLIENT;
          msg_buffer.header.length              = sizeof(firmware_update_metadata_t);
          msg_buffer.header.destination_address = BROADCAST_ADDRESS;
          firmware_update_metadata_t capability = get_capability(image_magic);
          memcpy((uint8_t*)msg_buffer.data, (uint8_t*)&capability, sizeof(firmware_update_metadata_t));
          send(reply_port, &msg_buffer, sizeof())
        }

        break;
      case FWU_STREAM_IMAGE:

        break;
      case FWU_VERIFY_IMAGE:
        break;
      case FWU_ERASE_METADATA:
        break;
      case FWU_WRITE_METADATA:
        break;
      default:
        break;
    }
  }
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  if (HAL_FLASH_ERROR_NONE == ReturnValue)
  {
    switch (fwu_state)
    {
      case FWU_ERASE_IMAGE:
        fwu_state                = FWU_STREAM_IMAGE;
        msg_buffer.header.status = BASIC_SYSTEM_MESSAGE_FW_UPDATE msg, send(reply_port, );
        firmware_update_handle_event();
        break;
      case FWU_ERASE_METADATA: // FWU
        if ((incoming_metadata.image_magic == IMAGE_MAGIC_APP) || (incoming_metadata.image_magic == IMAGE_MAGIC_BL))
        {
          // We have something to write so write it
          fwu_state         = FWU_WRITE_METADATA;
          flash_write_ptr   = (uint64_t)&incoming_metadata;
          flash_address     = get_meta_address(incoming_metadata.image_magic);
          flash_write_count = sizeof(incoming_metadata);
          HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, *flash_write_ptr);
        }
        else
        {
          fwu_state = FWU_IDLE;
          HAL_FLASH_Lock();
        }
        break;
      case FWU_WRITE_METADATA:
        flash_write_count -= sizeof(uint64_t);
        flash_address += sizeof(uint64_t);
        if (flash_write_count > 0)
        {
          HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, flash_address, *flash_write_ptr);
        }
        else
        {
          HAL_FLASH_Lock();
          fwu_state = FWU_IDLE;
          if (NULL != operation_callback)
          {
            operation_callback(ReturnValue, 0);
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
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, get_meta_address(incoming_metadata.image_magic), 0);
    HAL_FLASH_Lock();
    fwu_state = FWU_IDLE;
  }
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, get_meta_address(incoming_metadata.image_magic), 0);
  HAL_FLASH_Lock();
  fwu_state = FWU_IDLE;
}