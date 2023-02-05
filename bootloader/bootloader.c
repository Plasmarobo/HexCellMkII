#include "bootloader.h"

#include "basic_system.h"
#include "boot.h"
#include "communication.h"
#include "display.h"
#include "memory_layout.h"
#include "message_protocol.h"
#include "reset.h"
#include "serial_output.h"
#include "tim.h"

#include <stddef.h>
#include <stdint.h>

#define BOOTLOADER_FLAG_SENDING (1)
#define BOOT_ADDRESS_INVALID (0xFFFFFFFF)

__attribute__((section(".bootloader_meta"))) image_metadata_t image_metadata = {
  .image_magic       = IMAGE_MAGIC_BL,
  .version_major     = FW_VERSION_MAJOR,
  .version_minor     = FW_VERSION_MINOR,
  .version_patch     = FW_VERSION_PATCH,
  .hardware_revision = HW_VERSION,
  .reserved          = 0,
  .boot_address      = BOOT_ADDRESS,
};

typedef enum
{
  BOOT_STATE_LISTEN = 0,
  BOOT_STATE_UPDATE,
  BOOT_STATE_JUMP_TO_APP,
  BOOT_STATE_MAX,
} bootloader_state_t;

static bootloader_state_t boot_state;
static uint32_t           bootloader_flags;
static volatile bool      display_ready;

static void handle_reply_complete(int32_t status, uint32_t port)
{
  bootloader_flags &= ~BOOTLOADER_FLAG_SENDING;
}

static void handle_display_complete(int32_t status, uint32_t port)
{
  display_ready = true;
}

void bootloader_init(void)
{
  boot_mode_t    boot_mode    = get_boot_mode();
  reset_reason_t reset_reason = get_reset_reason();
  if (BOOT_MODE_BOOTLOADER == boot_mode)
  {
    if (RESET_REASON_FIRMWARE_UPDATE_START == reset_reason)
    {
      // Expect another message from the host
      boot_state = BOOT_STATE_UPDATE;
    }
    else
    {
      boot_state = BOOT_STATE_LISTEN;
    }
  }
  else
  {
    boot_state = BOOT_STATE_JUMP_TO_APP;
  }
  // DEV OVERRIDE: force boot state to listen for debugging purposes
  boot_state = BOOT_STATE_LISTEN;

  const image_metadata_t* bootloader_data = get_image_metadata(IMAGE_MAGIC_BL);

  serial_print("=== BOOTLOADER ===\r\n");
  serial_printf("Boot mode: %d, Reset reason: %d\r\n", boot_mode, reset_reason);
  if ((NULL == bootloader_data) || (bootloader_data->image_magic != IMAGE_MAGIC_BL))
  {
    serial_print("Bootloader metadata invalid\r\n");
  }
  else
  {
    serial_printf("Version %d.%d.%d\r\n",
                  bootloader_data->version_major,
                  bootloader_data->version_minor,
                  bootloader_data->version_patch);
    serial_printf("HW Rev: %d\r\n", bootloader_data->hardware_revision);
    serial_printf("Boot address: %x\r\n", bootloader_data->boot_address);
  }
  display_ready = true;
  display_init();
  display_clear();
  display_set_boot_pattern();
}

void bootloader_update(void)
{
  switch (boot_state)
  {
    case BOOT_STATE_LISTEN:
      if (!received_messages_empty() && !(bootloader_flags & BOOTLOADER_FLAG_SENDING))
      {
        // We currently only care about firmware update messages
        // Sink all other messages with a rejection reply
        receive_opt_t incoming_message = dequeue_message();
        if ((incoming_message.msg.header.status < BASIC_SYSTEM_MESSAGE_TYPE) ||
            (incoming_message.msg.header.status >= BASIC_SYSTEM_MESSAGE_MAX))
        {
          // Error reply
          message_t msg;
          msg.header.status = BASIC_SYSTEM_MESSAGE_NOT_IMPLEMENTED;
          msg.header.length = 0;
          bootloader_flags |= BOOTLOADER_FLAG_SENDING;
          queue_message_send(incoming_message.origin_port, &msg, handle_reply_complete);
        }
        else
        {
          // Pass the message to the handler
          basic_system_handle_message(incoming_message);
        }
      }

      if (display_ready)
      {
        display_ready = false;
        display_update(handle_display_complete);
      }
      break;
    case BOOT_STATE_JUMP_TO_APP:
      {
        // Read app metadata
        const image_metadata_t* app_meta = get_image_metadata(IMAGE_MAGIC_APP);
        if ((NULL != app_meta) && (IMAGE_MAGIC_APP == app_meta->image_magic))
        {
          if (BOOT_ADDRESS_INVALID != app_meta->boot_address)
          {
            // Metadata is telling us where to jump to, trust it
            serial_printf("LOADING %x\r\n", app_meta->boot_address);
            boot_jump_to_image((uint32_t*)app_meta->boot_address);
          }
          else
          {
            serial_printf("LOADING %x\r\n", ORIGIN_APP);
            // Jump to where we think the app should begin
            boot_jump_to_image((uint32_t*)ORIGIN_APP);
          }
        }
        else
        {
          // Todo: set error pattern
          serial_print("Invalid jump address, listening\r\n");
          boot_state = BOOT_STATE_LISTEN;
        }
      }
      break;
    default:
      break;
  }
}