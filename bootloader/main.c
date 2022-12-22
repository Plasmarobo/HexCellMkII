#include "bsp.h"

#include "crc.h"
#include "dma.h"
#include "gpio.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"

#include "basic_system.h"
#include "bootloader.h"
#include "communication.h"
#include "reset.h"
#include "time.h"

#include <stdbool.h>
#include <stdint.h>

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

  while (1)
  {
    communications_update();
    bootloader_update();
  }
}
