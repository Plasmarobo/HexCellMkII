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

#include "opt_prototypes.h"

#include "message_protocol.h"

#define MESSAGE_QUEUE_LENGTH (8)

typedef enum
{
  MESSAGE_OK = 0,
  MESSAGE_QUEUE_FULL,
  MESSAGE_ERROR,
} message_status_t;

/* Initialize communications structures */
void communications_init(void);
/* Update communications interfaces, may operate asynchronously */
void communications_update(opt_callback_t callback);
// Send message
message_status_t message_send(message_t* msg, opt_callback_t callback);
// Receive message
message_status_t message_receive(message_t* msg, opt_callback_t callback);

#endif