#include "comm_phy.h"

#include "gpio.h"
#include "message_protocol.h"
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "usart.h"

#include <stdint.h>

typedef struct
{
  message_t*     buffer;
  opt_callback_t callback;
} port_state_t;

static port_state_t     port_data[PORT_MAX] = { 0 };
static volatile uint8_t connected_status;
static opt_callback_t   connect_status_callback;

void handle_error(uint32_t port, int32_t status)
{
  if (NULL != port_data[port].callback)
  {
    port_data[port].callback(port, status);
  }
}

void read_body_callback(int32_t status, uint32_t port)
{
  if (port < PORT_MAX)
  {
    if (UART_SUCCESS != status)
    {
      handle_error(port, status);
    }
    else
    {
      if (NULL != port_data[port].callback)
      {
        // We shouldn't need to pass the buffer back, but do so just in case
        port_data[port].callback(port, (uint32_t)port_data[port].buffer);
        port_data[port].callback = NULL;
        // Do not listen again: we may want to process or copy the data, allow the CB to start the next listen
      }
    }
  }
}

void listen_callback(int32_t status, uint32_t port)
{
  if (port < PORT_MAX)
  {
    if (UART_SUCCESS != status)
    {
      // Abort and propogate error
      handle_error(port, status);
    }
    else
    {
      // We've read the length, gather the remaining bytes
      uart_receive(port, port_data[port].buffer->data, port_data[port].buffer->header.length, read_body_callback);
    }
  }
}

void write_body_callback(int32_t status, uint32_t port)
{
  if (port < PORT_MAX)
  {
    if (UART_SUCCESS != status)
    {
      handle_error(port, status);
    }
    else
    {
      if (NULL != port_data[port].callback)
      {
        // We shouldn't need to send the buffer back, but do so just in case
        port_data[port].callback(port, (uint32_t)&port_data[port].buffer);
        port_data[port].callback = NULL;
      }
    }
  }
}

void send_callback(int32_t status, uint32_t port)
{
  if (port < PORT_MAX)
  {
    if (UART_SUCCESS != status)
    {
      handle_error(port, status);
    }
    else
    {
      uart_send(port, port_data[port].buffer->data, port_data[port].buffer->header.length, write_body_callback);
    }
  }
}

void comm_phy_init(void)
{
  // Initial query of ports
  connected_status = 0;
  if (GPIO_PIN_SET == HAL_GPIO_ReadPin(DETECT_A_GPIO_Port, DETECT_A_Pin))
  {
    connected_status |= COMM_PORT_MASK(PORT_AO);
  }
  if (GPIO_PIN_SET == HAL_GPIO_ReadPin(DETECT_B_GPIO_Port, DETECT_B_Pin))
  {
    connected_status |= COMM_PORT_MASK(PORT_BO);
  }
  if (GPIO_PIN_SET == HAL_GPIO_ReadPin(DETECT_C_GPIO_Port, DETECT_C_Pin))
  {
    connected_status |= COMM_PORT_MASK(PORT_CO);
  }
}

// Read the header and body from the port, return final body buffer
void listen(comm_port_t port, message_t* buffer, opt_callback_t cb)
{
  if ((port < PORT_MAX) && (NULL != buffer))
  {
    port_data[port].callback = cb;
    port_data[port].buffer   = buffer;
    // Listen for incoming transaction length
    uart_receive(port, (uint8_t*)&(port_data[port].buffer->header), sizeof(message_header_t), listen_callback);
  }
}
/* Send data to a port */
void send(comm_port_t port, message_t* buffer, opt_callback_t cb)
{
  // Don't send if we don't have something connected
  if ((port < PORT_MAX) && is_connected(port) && (NULL != buffer) && (0 < buffer->header.length))
  {
    port_data[port].callback = cb;
    port_data[port].buffer   = buffer;
    uart_send(port, (uint8_t*)&(port_data[port].buffer->header), sizeof(message_header_t), send_callback);
  }
}

bool port_busy(comm_port_t port)
{
  if (port < PORT_MAX)
  {
    return NULL != port_data[port].callback;
  }
  return true;
}

bool is_connected(comm_port_t port)
{
  return COMM_PORT_MASK(port) & connected_status;
}

void on_connect_disconnect(opt_callback_t cb)
{
  connect_status_callback = cb;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  comm_port_t port      = PORT_MAX;
  GPIO_TypeDef* gpio_port = NULL;
  switch (GPIO_Pin)
  {
    case DETECT_B_Pin:
      port      = PORT_BO;
      gpio_port = DETECT_B_GPIO_Port;
      break;
    case DETECT_C_Pin:
      port      = PORT_CO;
      gpio_port = DETECT_C_GPIO_Port;
      break;
    case DETECT_A_Pin:
      port      = PORT_AO;
      gpio_port = DETECT_A_GPIO_Port;
      break;
    default:
      break;
  }
  uint8_t value = HAL_GPIO_ReadPin(gpio_port, GPIO_Pin);
  uint8_t mask  = COMM_PORT_MASK(port);
  if (GPIO_PIN_SET == value)
  {
    connected_status |= mask;
  }
  else
  {
    // Handle PHY disconnect
    connected_status &= ~mask;
    uart_abort_tx(port);
  }
  if ((PORT_MAX != port) && (NULL != connect_status_callback))
  {
    connect_status_callback(port, (uint32_t)value);
  }
}