#ifndef COMMUNICATION_H
#define COMMUNICATION_H
/**
 * @file communication.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Defines the logical communications layer.
 * This sends raw messages to an immediate graph neighbor
 */

#include "basic_system.h"
#include "comm_phy.h"
#include "error.h"
#include "message_protocol.h"
#include "opt_prototypes.h"

#include <stdbool.h>
#include <stdint.h>

// Warning: this will consume more than 3k per point of depth
#define TX_QUEUE_LENGTH (4)
#define RX_QUEUE_LENGTH (12)

typedef enum
{
  COMM_ERROR_NONE             = 0,
  COMM_ERROR_FAILURE_TO_QUEUE = COMM_ERROR,
  COMM_ERROR_INCOMING_MESSAGE_DROP,
  COMM_ERROR_OUTGOING_MESSAGE_DROP,
  COMM_ERROR_INTERNAL_MESSAGE_DROP,
  COMM_ERROR_INVALID_STATUS,
  COMM_ERROR_INVALID_LENGTH,
} comm_error_code_t;

typedef enum
{
  COMM_ID_QUERY = COMM_MESSAGE_TYPE,
  COMM_ID_DATA,
  COMM_PORT_CONNECTED,
} comm_message_type_t;

typedef struct __attribute__((packed, aligned(8)))
{
  // The hardware ID
  uint32_t id;
  // Logical/network address
  uint32_t address;
  // Version information
  uint8_t version_major;
  uint8_t version_minor;
  uint8_t version_patch;
  uint8_t reserved;
} port_description_t;

/* Initialize communications structures */
void communications_init(uint32_t self_address);
/* Update communications interfaces, may operate asynchronously */
void communications_update(void);
// Queue message for send, msg does not need to exist after call
void queue_message_send(comm_port_t port, message_t* msg, opt_callback_t callback);
// Is the queue empty
bool received_messages_empty(void);
// Pop incoming message queue (to process in the network layer)
receive_opt_t dequeue_message(void);

#endif