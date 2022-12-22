#include "basic_system.h"
#include "bsp.h"
#include "memory_layout.h"

#include "cmsis_os.h"

__attribute__((section(".app_meta"))) image_metadata_t image_metadata = {
  .image_magic       = IMAGE_MAGIC_APP,
  .version_major     = FW_VERSION_MAJOR,
  .version_minor     = FW_VERSION_MINOR,
  .version_patch     = FW_VERSION_PATCH,
  .hardware_revision = HW_VERSION,
  .reserved          = 0,
  .boot_address      = 0,
};

void MX_FREERTOS_Init(void);

int main(void)
{
  board_init();
  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
  return 0;
}