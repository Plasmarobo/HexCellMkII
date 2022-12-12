#include "bootloader.h"

#include "basic_system.h"
#include "boot.h"
#include "communication.h"
#include "memory_layout.h"
#include "message_protocol.h"
#include "reset.h"

#include <stddef.h>
#include <stdint.h>

#define BOOTLOADER_FLAG_SENDING (1)
#define BOOT_ADDRESS_INVALID (0xFFFFFFFF)

typedef enum
{
  BOOT_STATE_LISTEN = 0,
  BOOT_STATE_UPDATE,
  BOOT_STATE_JUMP_TO_APP,
  BOOT_STATE_MAX,
} bootloader_state_t;

static bootloader_state_t boot_state;
static uint32_t           bootloader_flags;

static void handle_reply_complete(int32_t status, uint32_t port)
{
  bootloader_flags &= ~BOOTLOADER_FLAG_SENDING;
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
            boot_jump_to_image((uint32_t*)app_meta->boot_address);
          }
          else
          {
            // Jump to where we think the app should begin
            boot_jump_to_image((uint32_t*)_ORIGIN_APP);
          }
        }
        else
        {
          // Todo: set error pattern
          boot_state = BOOT_STATE_LISTEN;
        }
      }
      break;
    default:
      break;
  }
}