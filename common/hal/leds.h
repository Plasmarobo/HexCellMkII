#ifndef LEDS_H
#define LEDS_H
/**
 * @file leds.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * This file implements a standard interface for a cluster of chained
 * RGB leds, each BSP must implement this interface
 */
#include "opt_prototypes.h"
#include "version.h"

#include <stdint.h>

/**
 * Table of HW versions
 * 0  - MkI-R1 (3 leds)
 * 1  - MkI-R2 (6 leds)
 * 2  - MkI-R3 (6 leds)
 * 3  - MkI-R4 (7 leds)
 * 4  - MkII-R1 (7 leds)
 * 5  - MkII-R1C (7 leds)
 * 6  - MkII-R2 (7 leds)
 * 7  - MkII-R3 (6 leds)
 * 8  - MkIII (N/A)
 * 9  - MkIV-R1 (6 leds)
 * 10 - MkV-R1 (7 leds)
 * 11 - MkV-R1L (6 leds)
 */
#if (HW_VERSION == HW_VERSION_MKI)
#define LED_COUNT (3)
#elif (HW_VERSION > HW_VERSION_MKI) && (HW_VERSION <= HW_VERSION_MKIR3)
#define LED_COUNT (6)
#elif (HW_VERSION > HW_VERSION_MKIR3) && (HW_VERSION <= HW_VERSION_MKIIR2)
#define LED_COUNT (7)
#elif (HW_VERSION > HW_VERSION_MKIIR2) && (HW_VERSION <= HW_VERSION_MKIVR1)
#define LED_COUNT (6)
#elif (HW_VERSION > HW_VERSION_MKIVR1) && (HW_VERSION <= HW_VERSION_MKVR1)
#define LED_COUNT (7)
#elif (HW_VERSION > HW_VERSION_MKVR1) && (HW_VERSION <= HW_VERSION_MKVR1L)
#define LED_COUNT (6)
#else
#error "Invalid HW_VERSION, unknown LED_COUNT, define leds for hardware version"
#endif

void set_rgb(uint8_t address, uint8_t r, uint8_t g, uint8_t b);
void set_hsv(uint8_t address, uint16_t h, uint8_t s, uint8_t v);
void update_leds(opt_callback_t cb);
void lock_leds(void);
void unlock_leds(void);

#endif // LEDS_H
