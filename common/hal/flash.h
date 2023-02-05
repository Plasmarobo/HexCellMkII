#ifndef FLASH_H
#define FLASH_H

#include "opt_prototypes.h"

#include <stdint.h>

#define FLASH_ERROR (-2)
#define FLASH_BUSY (-1)
#define FLASH_SUCCESS (0)

void      flash_erase(uint32_t address, uint32_t length, opt_callback_t cb);
void      flash_write(uint32_t address, uint8_t* data, uint32_t length, opt_callback_t cb);
uintptr_t flash_read(uint32_t address);
void      flash_init(void);

#endif