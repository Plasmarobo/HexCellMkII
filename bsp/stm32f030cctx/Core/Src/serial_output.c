#include "serial_output.h"

#include "usart.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_LEN (256)

static char          buffer[BUFFER_LEN];
static volatile bool lock = false;

static void handle_send_complete(int32_t status, uint32_t len)
{
  lock = false;
}

// Route to BI
int serial_print(const char* str)
{
  while (lock)
  {
    // spinlock
  };
  lock    = true;
  int len = snprintf(buffer, BUFFER_LEN, "%s", str);
  uart_send(PORT_BI, (uint8_t*)buffer, len, handle_send_complete);
  return len;
}

int serial_printf(const char* format, ...)
{
  while (lock)
  {
    // spinlock
  };
  lock = true;
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buffer, BUFFER_LEN, format, args);
  uart_send(PORT_BI, (uint8_t*)buffer, len, handle_send_complete);
  return len;
}
