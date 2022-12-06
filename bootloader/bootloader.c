#include "bootloader.h"

#include "basic_system.h"
#include "boot.h"
#include "communication.h"
#include "memory_layout.h"
#include "message_protocol.h"
#include "reset.h"

#define BOOTLOADER_FLAG_SENDING (1)

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
      boot_jump_to_image((uint32_t*)_ORIGIN_APP);
      break;
    default:
      break;
  }
}