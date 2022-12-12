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
#include "reset.h"
#include "time.h"

#include <stdbool.h>
#include <stdint.h>

#define DISPLAY_LIMIT_MS (10)

volatile bool   display_ready       = true;
static uint32_t display_limit_timer = 0;

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

  display_set_boot_pattern();
  display_limit_timer = 0;

  while (1)
  {
    if (display_ready && ((get_milliseconds() - display_limit_timer) > DISPLAY_LIMIT_MS))
    {
      display_ready = false;
      display_update(handle_display_update);
    }
    communications_update();
    bootloader_update();
  }
}

void handle_display_update(int32_t result, uint32_t userdata)
{
  display_ready       = true;
  display_limit_timer = get_milliseconds();
}