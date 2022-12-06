#include "bsp.h"

#include "crc.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"

#include "basic_system.h"
#include "bootloader.h"
#include "communication.h"
#include "display.h"
#include "leds.h"
#include "reset.h"

#include <stdbool.h>
#include <stdint.h>

volatile bool display_ready = true;

void handle_display_update(int32_t result, uint32_t userdata);

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  reset_init();
  board_init();
  communications_init(BOOTLOADER_ADDRESS);
  bootloader_init();

  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 128;

  for (uint8_t i = 0; i < LED_COUNT; ++i)
  {
    set_pattern_rgb(PATTERN_HEARTBEAT, i, r, g, b);
    uint8_t tmp = b;
    b           = g;
    g           = r;
    r           = tmp;
  }

  while (1)
  {
    communications_update();
    if (display_ready)
    {
      display_ready = false;
      display_update(handle_display_update);
    }
    bootloader_update();
  }
}

void handle_display_update(int32_t result, uint32_t userdata)
{
  display_ready = true;
}