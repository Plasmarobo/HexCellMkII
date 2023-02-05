#ifndef RESET_H
#define RESET_H
/**
 * @file reset.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Interfaces and access to reset information
 * There is a region of ram that may be loaded with metadata
 */

#include <stdint.h>

#define RESET_VALID (0x600D)
#define RESET_INVALID (0x1515)

typedef enum
{
  RESET_REASON_INVALID = 0,
  RESET_REASON_LOW_POWER,
  RESET_REASON_POWER_ONDOWN,
  RESET_REASON_IWDG,
  RESET_REASON_WWDG,
  RESET_REASON_SOFTWARE,
  RESET_REASON_HARD_RESET,
  RESET_REASON_SOFT_RESET,
  RESET_REASON_PIN,
  RESET_REASON_FIRMWARE_UPDATE_START,
  RESET_REASON_FIRMWARE_UPDATE_COMPLETE,
  RESET_REASON_OTHER,
  RESET_REASON_MAX,
} reset_reason_t;

typedef enum
{
  BOOT_MODE_INVALID = 0,
  BOOT_MODE_BOOTLOADER,
  BOOT_MODE_APP,
  BOOT_MODE_MAX
} boot_mode_t;

typedef struct __attribute__((packed, aligned(4)))
{
  uint16_t validation_code;
  uint8_t  reset_code;
  uint8_t  boot_mode;
  uint32_t reset_flags;
} reset_info_t;

extern reset_info_t reset_breadcrumb;

void           reset_init(void);
reset_reason_t get_reset_reason(void);
uint32_t       get_reset_flags(void);
boot_mode_t    get_boot_mode(void);

void hard_reset(uint8_t reset_code, uint8_t boot_mode);
void soft_reset(uint8_t reset_code, uint8_t boot_mode);

#endif