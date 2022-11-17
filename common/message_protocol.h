#ifndef MESSAGE_PROTOCOL_H
#define MESSAGE_PROTOCOL_H
/**
 * @file message_protocol.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Defines message structures to be passed through the graph
 */

#include <stdint.h>

#define BROADCAST_ADDRESS (0)
#define INVALID_ADDRESS (0xFFFFFFFF)

#define MESSAGE_BUFFER_LENGTH (512)

typedef struct
{
  uint16_t status;
  uint16_t length;
  uint32_t destination_address;
} message_header_t;

typedef struct
{
  message_header_t header;
  uint8_t          data[MESSAGE_BUFFER_LENGTH];
} message_t;

typedef enum
{
  // Reserve 0x0-0x0F for system messages
  BASIC_SYSTEM_MESSAGE_TYPE = 0x00,
  // Reserve 0x10-0x1F for boot messages
  BOOT_MESSAGE_TYPE = 0x10,
  // Reserve 0x20- for app messages
  APP_MESSAGE_TYPE = 0x20,
} message_type_t;

#endif // _MESSAGE_PROTOCOL_H_