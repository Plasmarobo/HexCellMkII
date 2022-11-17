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

#include <stdint.h>

#if HW_VERSION < 4
#define LED_COUNT (6)
#else
#define LED_COUNT (7)
#endif

void set_rgb(uint8_t address, uint8_t r, uint8_t g, uint8_t b);
void set_hsv(uint8_t address, uint16_t h, uint8_t s, uint8_t v);
void update_leds(opt_callback_t cb);
void lock_leds(void);
void unlock_leds(void);

#endif // LEDS_H
