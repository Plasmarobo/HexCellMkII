/**
 * @file communication.c
 * @author Austen Danger Bartels
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * This file implements the communications stack.
 * This is a logical layer that provides buffering and otherwise sources and sinks
 * data for the comm_phy layer (defined in the BSP)
 *
 * We reasonably expect a transparent send/receive interface here, but we need to
 * tolerate physical errors, memory errors, and timing errors. This layer
 * is responsible for logical error recovery and the resetting of the physical device.
 *
 * Buffers are expected to be owned (and un-modified) until the callback has fired
 * and returned.
 */

#include "communication.h"

#include "comm_phy.h"
#include "gpio.h"

#include <stdint.h>

typedef enum
{
  COMM_STATE_IDLE = 0,
  COMM_STATE_TRANSMIT,
  COMM_STATE_RECEIVE,
  COMM_STATE_ERROR
} comm_state_t;

typedef struct
{
  uint8_t      buffer[COMM_BUFFER_LENGTH];
  uint16_t     length;
  comm_state_t state;
} comm_data_t;

// Scratch buffer to unblock the comm_phy while we're working
static comm_data_t    port_state[PORT_MAX];
static opt_callback_t message_handler;

// Handle port listen complete
void handle_listen(int32_t status, uint32_t port)
{
  if (0 < status)
  {
    port_state[port].state = COMM_STATE_IDLE;
    // We should have a complete message here
    if (NULL != message_handler)
    {
      message_handler(status, port_state[port].buffer);
    }
  }
  port_state[port].length = 0;
  switch (port)
  {
    case PORT_AI: // Intentional fallthrough
    case PORT_BI: // Intentional fallthrough
    case PORT_CI:
      port_state[port].state = COMM_STATE_RECEIVE;
      listen(port, port_state[port], handle_listen);
      break;
    default: // We don't want to listen on OUTPUT ports
      break;
  }
}

// Handle port reply complete
void handle_reply(int32_t status, uint32_t port)
{
}

void connect_disconnect_callback(int32_t port, uint32_t connect_status)
{
  if (GPIO_PIN_SET == connect_status)
  {
    // CONNECT event on an output port
    // send query
    if (COMM_STATE_IDLE != port_state[port].status)
    {
      
      send(port, )
    }
  }
  else
  {
    // DISCONNECT event on an output port
    // comm phy should reset itself, but we need to clear our state
    if (COMM_STATE_IDLE != port_state[port].status)
    {
    }
  }
}

void communications_init(opt_callback_t message_callback)
{
  // Listen on all input interfaces
  message_handler           = message_callback;
  port_state[PORT_AO].state = COMM_STATE_IDLE;
  port_state[PORT_BO].state = COMM_STATE_IDLE;
  port_state[PORT_CO].state = COMM_STATE_IDLE;

  port_state[PORT_AI].state = COMM_STATE_RECEIVE;
  listen(PORT_AI, port_state[PORT_AI].buffer, handle_listen);
  port_state[PORT_BI].state = COMM_STATE_RECEIVE;
  listen(PORT_BI, port_state[PORT_BI].buffer, handle_listen);
  port_state[PORT_CI].state = COMM_STATE_RECEIVE;
  listen(PORT_CI, port_state[PORT_CI].buffer, handle_listen);
  on_connect_disconnect(connect_disconnect_callback);
}

void communications_update(opt_callback_t callback)
{
}