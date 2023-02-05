#include "serial_output.h"

#include "usart.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef SERIAL_OUTPUT_ENABLED

#define BUFFER_LEN (26)
#define DEBUG_PORT (PORT_USB)

static char          buffer[BUFFER_LEN];
static volatile bool lock = false;

static void handle_send_complete(int32_t status, uint32_t len)
{
  lock = false;
}
#endif

int serial_print(const char* str)
{
#ifdef SERIAL_OUTPUT_ENABLED
  while (lock)
  {
    // spinlock
  };
  lock    = true;
  int len = snprintf(buffer, BUFFER_LEN, "%s", str);
  uart_send(DEBUG_PORT, (uint8_t*)buffer, len, handle_send_complete);
  return len;
#else
  return 0;
#endif
}

int serial_printf(const char* format, ...)
{
#ifdef SERIAL_OUTPUT_ENABLED
  while (lock)
  {
    // spinlock
  };
  lock = true;
  va_list args;
  va_start(args, format);
  int len = vsnprintf(buffer, BUFFER_LEN, format, args);
  uart_send(DEBUG_PORT, (uint8_t*)buffer, len, handle_send_complete);
  return len;
#else
  return 0;
#endif
}