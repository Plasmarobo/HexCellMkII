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

#include "basic_system.h"
#include "comm_phy.h"
#include "gpio.h"
#include "message_protocol.h"
#include "report.h"

#include "stm32f0xx_hal.h"

#include "FIFO.h"

#include <stdint.h>
#include <string.h>

// Queue of outgoing messages for sending
typedef struct
{
  fifo_t                 queue;
  struct fifo_descriptor queue_impl;
  uint8_t                queue_buffer[TX_QUEUE_LENGTH * sizeof(send_opt_t)];
} tx_queue_t;

typedef struct
{
  fifo_t                 queue;
  struct fifo_descriptor queue_impl;
  uint8_t                queue_buffer[RX_QUEUE_LENGTH * sizeof(receive_opt_t)];
} rx_queue_t;

static tx_queue_t         send_queue[PORT_MAX];
static rx_queue_t         receive_queue;
static message_t          port_buffer[PORT_MAX];
static port_description_t port_description[PORT_MAX];
static uint32_t           self_address;

static port_description_t get_own_description(void)
{
  port_description_t own_description;
  own_description.id            = get_hardware_id();
  own_description.address       = self_address;
  own_description.version_major = current_metadata->version_major;
  own_description.version_minor = current_metadata->version_minor;
  own_description.version_patch = current_metadata->version_patch;
  return own_description;
}

// Handle unsolicited data
void handle_listen(int32_t port, uint32_t status)
{
  if (port < PORT_MAX)
  {
    if (COMM_SUCCESS == status)
    {
      // Copy buffer to rx queue
      if (!fifo_add(receive_queue.queue, &port_buffer[port]))
      {
        // Queue failure, all we can do is drop the message
        report_error(RT_WARNING, COMM_ERROR_INCOMING_MESSAGE_DROP, 0, port);
      }
      port_buffer[port].header.length = 0;
      // listen(port, &port_buffer[port], handle_listen);
    }
    else
    {
      report_error(RT_ERROR, COMM_ERROR_HW_ERROR, status, port);
    }
  }
}

void handle_connect_reply(int32_t port, uint32_t buffer_address)
{
  if (port < PORT_MAX)
  {
    if (COMM_ERROR_NONE != port_buffer[port].header.status)
    {
      report_error(RT_WARNING, COMM_ERROR_INVALID_STATUS, port_buffer[port].header.status, port);
    }
    else if (port_buffer[port].header.length < sizeof(port_description_t))
    {
      report_error(RT_WARNING, COMM_ERROR_INVALID_LENGTH, port_buffer[port].header.length, port);
    }
    else
    {
      // Populate the port descriptior
      memcpy((uint8_t*)&port_description[port], (uint8_t*)&port_buffer[port], sizeof(port_description_t));
      // Repackage the message for upper layers
      receive_opt_t opt = {
        .msg = {
          .header = {
            .status = COMM_PORT_CONNECTED,
            .length = sizeof(port_description_t),
          },
        },
        .origin_port = port,
      };
      memcpy(opt.msg.data, (uint8_t*)&(port_description[port]), sizeof(port_description_t));
      if (!fifo_add(receive_queue.queue, &opt))
      {
        report_error(RT_WARNING, COMM_ERROR_INTERNAL_MESSAGE_DROP, 0, PORT_MAX);
      }
    }
  }
}

void handle_connect_query(int32_t port, uint32_t buffer_address)
{
  if (port < PORT_MAX)
  {
    listen(port, &port_buffer[port], handle_connect_reply);
  }
}

void connect_disconnect_callback(int32_t port, uint32_t connect_status)
{
  if (GPIO_PIN_SET == connect_status)
  {
    // CONNECT event on an output port
    // send query
    port_buffer[port].header.status              = COMM_ID_QUERY;
    port_buffer[port].header.length              = sizeof(port_description_t);
    port_buffer[port].header.destination_address = 0;
    port_description_t self                      = get_own_description();
    memcpy(port_buffer[port].data, (uint8_t*)&self, sizeof(port_description_t));
    send(port, &port_buffer[port], handle_connect_query);
  }
  else
  {
    // DISCONNECT event on an output port
    // comm phy should reset itself, but we need to clear our state
    port_buffer[port].header.length = 0;
    port_buffer[port].header.status = 0;
  }
}

void communications_init(uint32_t address)
{
  self_address = address;
  for (uint8_t i = PORT_AO; i < PORT_MAX; ++i)
  {
    send_queue[i].queue = fifo_create_static(&(send_queue[i].queue_impl),
                                             send_queue[i].queue_buffer,
                                             TX_QUEUE_LENGTH,
                                             sizeof(send_opt_t));
    if (NULL != send_queue[i].queue)
    {
      listen(i, &(port_buffer[i]), handle_listen);
    }
  }
  receive_queue.queue = fifo_create_static(&receive_queue.queue_impl, receive_queue.queue_buffer, RX_QUEUE_LENGTH, sizeof(receive_opt_t));
  if (NULL == receive_queue.queue)
  {
    Error_Handler();
  }
  on_connect_disconnect(connect_disconnect_callback);
}

void communications_update(void)
{
  // Scan ports, dequeue any queued messages to free ports
  for (uint8_t port = PORT_AO; port < PORT_MAX; ++port)
  {
    // Check bank signal lines
    if (!port_busy(port) && !fifo_is_empty(send_queue[port].queue))
    {
      send_opt_t opt;
      if (fifo_get(send_queue[port].queue, &opt))
      {
        port_buffer[port] = opt.msg;
        send(port, &port_buffer[port], opt.cb);
      }
      else
      {
        report_error(RT_WARNING, COMM_ERROR_INTERNAL_MESSAGE_DROP, 0, port);
      }
    }
  }
}

void queue_message_send(comm_port_t port, message_t* msg, opt_callback_t callback)
{
  if (NULL != msg)
  {
    // is the phy layer busy?
    if (!port_busy(port))
    {
      memcpy(&port_buffer[port], msg, sizeof(message_t));
      send(port, &port_buffer[port], callback);
    }
    else
    {
      send_opt_t opt = {
        .msg = *msg,
        .cb  = callback,
      };
      if (!fifo_add(send_queue[port].queue, &opt))
      {
        report_error(RT_WARNING, COMM_ERROR_OUTGOING_MESSAGE_DROP, 0, port);
      }
    }
  }
}

bool received_messages_empty(void)
{
  return fifo_is_empty(receive_queue.queue);
}

receive_opt_t dequeue_message(void)
{
  receive_opt_t msg;
  if (!fifo_is_empty(receive_queue.queue))
  {
    fifo_get(receive_queue.queue, &msg);
  }
  return msg;
}