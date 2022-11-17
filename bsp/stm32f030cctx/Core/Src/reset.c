/*!*****************************************************************************
 * @file
 *
 * @brief This file contains the hal implementation of the reset interface
 *        for the stm32f4xx series
 *
 * @copyright Superpedestrian Inc. (c) 2019
 *
 *******************************************************************************/

#include "reset.h"

#include <stdbool.h>
#include <stdint.h>

#include "stm32f0xx_hal.h"

_Static_assert(0 == offsetof(reset_info_t, validation_code), "validation code must be at offset 0.");
_Static_assert(2 == offsetof(reset_info_t, reset_code), "reset code must be at offset 2.");
_Static_assert(3 == offsetof(reset_info_t, boot_mode), "boot mode code must be at offset 3.");
_Static_assert(4 == offsetof(reset_info_t, reset_flags), "reset flags must be at offset 4.");

typedef struct
{
  reset_info_t*  p_info;
  uint32_t       reset_flags;
  reset_reason_t reason;
  boot_mode_t    boot_mode;
} reset_data_t;

static reset_data_t reset_data;

__attribute__((section(".reset_info"))) reset_info_t reset_breadcrumb;

static void           save_mcu_flags(reset_info_t* p_info);
static void           prepare_for_reset(uint8_t reset_code, uint8_t boot_mode);
static reset_reason_t get_reason_from_flags(uint32_t flags);

void reset_init(reset_info_t* p_info)
{
  reset_data.p_info = p_info;

  if ((reset_data.p_info != NULL) && (RESET_VALID == reset_data.p_info->validation_code))
  {
    reset_data.reason      = reset_data.p_info->reset_code;
    reset_data.boot_mode   = reset_data.p_info->boot_mode;
    reset_data.reset_flags = reset_data.p_info->reset_flags;

    if (RESET_REASON_OTHER == reset_data.reason)
    {
      reset_data.reason = decode_reset_flags(reset_data.reset_flags);
    }
  }
  else
  {
    reset_data.reset_flags = RCC->CSR;
    reset_data.reason      = decode_reset_flags(reset_data.reset_flags);
  }

  // Now clean up the reset artifacts
  if (reset_data.p_info != NULL)
  {
    reset_data.p_info->validation_code = RESET_INVALID;
    reset_data.p_info->reset_code      = RESET_REASON_OTHER;
    reset_data.p_info->boot_mode       = BOOT_MODE_INVALID;
  }
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

void save_mcu_flags(reset_info_t* p_info)
{
  // always save reset flags
  p_info->reset_flags = RCC->CSR;

  if ((p_info != NULL) && (RESET_VALID != p_info->validation_code))
  {
    p_info->boot_mode       = BOOT_MODE_INVALID;
    p_info->reset_code      = RESET_REASON_OTHER;
    p_info->validation_code = RESET_VALID;
  }
}

reset_reason_t get_reset_reason(void)
{
  return reset_data.reason;
}

uint32_t get_reset_flags(void)
{
  return reset_data.reset_flags;
}

void hard_reset(uint8_t reset_code, uint8_t boot_mode)
{
  prepare_for_reset(reset_code, boot_mode);
  NVIC_SystemReset();
}

boot_mode_t get_boot_mode(void)
{
  return reset_data.boot_mode;
}

void soft_reset(uint8_t reset_code, uint8_t boot_mode)
{
  prepare_for_reset(reset_code, boot_mode);
  boot_jump_to_image((uint32_t*)FLASH_BASE);
}

//------------------------------------
// Private Functions
//------------------------------------

/**
 * @brief Performs the setup for a reset
 * @param reset_code The reset code breadcrumb to leave behind
 * @param boot_mode The boot mode breadcrumb to leave behind
 */
static void prepare_for_reset(uint8_t reset_code, uint8_t boot_mode)
{
  // Disable interrupts and leave them disabled until reset
  bool atomic_status = hal_interrupt_disable();
  UNUSED(atomic_status);

  if ((NULL != reset_data.p_info) && (reset_code < RESET_REASON_MAX) && (reset_code > RESET_REASON_INVALID) &&
      (boot_mode < BOOT_MODE_MAX) && (boot_mode > BOOT_MODE_INVALID))
  {
    reset_data.p_info->validation_code = RESET_VALID;
    reset_data.p_info->reset_code      = reset_code;
    reset_data.p_info->boot_mode       = boot_mode;
  }
}

static reset_reason_t get_reset_flags(uint32_t reset_flags)
{
  reset_reason_t reason = RESET_REASON_OTHER;

  // Often more than one reset flag is set at once, so the following checks are priority-ordered.
  if (reset_flags & RCC_CSR_LPWRRSTF_Msk)
  {
    reason = RESET_REASON_LOW_POWER;
  }
  else if (reset_flags & RCC_CSR_PORRSTF_Msk)
  {
    reset_data.reason = RESET_REASON_POWER_ONDOWN;
  }
  else if (reset_flags & RCC_CSR_IWDGRSTF_Msk)
  {
    reason = RESET_REASON_IWDG;
  }
  else if (reset_flags & RCC_CSR_SFTRSTF_Msk)
  {
    // Check this before WWDG because a debugger reset sets both flags
    // and this is a more accurate cause than WWDG
    reason = RESET_REASON_SOFTWARE;
  }
  else if (reset_flags & RCC_CSR_WWDGRSTF_Msk)
  {
    // Check this before PIN because a watchdog reset sets both flags
    // and this is a more accurate cause than PIN
    reason = RESET_REASON_WWDG;
  }
  else if (reset_flags & RCC_CSR_PINRSTF_Msk)
  {
    reason = RESET_REASON_PIN;
  }

  return reason;
}
