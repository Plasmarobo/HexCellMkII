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

#include "boot.h"
#include "bsp.h"

#include <stdbool.h>
#include <stdint.h>

#include "stm32f1xx_hal.h"
#include "stm32f1xx_it.h"

_Static_assert(0 == offsetof(reset_info_t, validation_code), "validation code must be at offset 0.");
_Static_assert(2 == offsetof(reset_info_t, reset_code), "reset code must be at offset 2.");
_Static_assert(3 == offsetof(reset_info_t, boot_mode), "boot mode code must be at offset 3.");
_Static_assert(4 == offsetof(reset_info_t, reset_flags), "reset flags must be at offset 4.");

__attribute__((section(".reset_info"))) reset_info_t reset_breadcrumb;

static void           prepare_for_reset(uint8_t reset_code, uint8_t boot_mode);
static reset_reason_t get_reason_from_flags(uint32_t flags);

void reset_init(void)
{
  // Always read reset register and update with hardware flags
  reset_breadcrumb.reset_flags = RCC->CSR;
  if (RESET_VALID != reset_breadcrumb.validation_code)
  {
    // Junk data, reset and proceed
    reset_breadcrumb.validation_code = RESET_INVALID;
    reset_breadcrumb.boot_mode       = BOOT_MODE_INVALID;
    reset_breadcrumb.reset_code      = get_reason_from_flags(reset_breadcrumb.reset_flags);
  }
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

reset_reason_t get_reset_reason(void)
{
  return reset_breadcrumb.reset_code;
}

uint32_t get_reset_flags(void)
{
  return reset_breadcrumb.reset_flags;
}

void hard_reset(uint8_t reset_code, uint8_t boot_mode)
{
  prepare_for_reset(reset_code, boot_mode);
  NVIC_SystemReset();
}

boot_mode_t get_boot_mode(void)
{
  return reset_breadcrumb.boot_mode;
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

  if ((reset_code < RESET_REASON_MAX) && (reset_code > RESET_REASON_INVALID) && (boot_mode < BOOT_MODE_MAX) &&
      (boot_mode > BOOT_MODE_INVALID))
  {
    reset_breadcrumb.validation_code = RESET_VALID;
    reset_breadcrumb.reset_code      = reset_code;
    reset_breadcrumb.boot_mode       = boot_mode;
  }
  else
  {
    reset_breadcrumb.validation_code = RESET_INVALID;
  }
}

static reset_reason_t get_reason_from_flags(uint32_t reset_flags)
{
  reset_reason_t reason = RESET_REASON_OTHER;

  // Often more than one reset flag is set at once, so the following checks are priority-ordered.
  if (reset_flags & RCC_CSR_LPWRRSTF_Msk)
  {
    reason = RESET_REASON_LOW_POWER;
  }
  else if (reset_flags & RCC_CSR_PORRSTF_Msk)
  {
    reason = RESET_REASON_POWER_ONDOWN;
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
