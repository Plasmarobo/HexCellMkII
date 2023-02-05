
#ifndef DISPLAY_H
#define DISPLAY_H
/**
 * @file display.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Defines the interface to the logical display manager.
 * This provides pattern handlers. The update function may be called in a
 * loop, or RTOS task
 */
#include "opt_prototypes.h"

#include <stdint.h>

#define DISPLAY_ENABLED

typedef enum
{
  PATTERN_NONE = 0,
  PATTERN_SOLID,
  PATTERN_BREATHE,
  PATTERN_HEARTBEAT,
  PATTERN_BLINK,
  PATTERN_SOS,
  PATTERN_CHASE, // Set all leds as chase: each led will be calculated independently
  PATTERN_MAX,
} pattern_t;

/* Initialize display structures */
void display_init(void);
/* Update pattern engine & leds, may operate asynchronously */
void display_update(opt_callback_t callback);
void display_set_hsv(uint8_t led, uint8_t h, uint8_t s, uint8_t v);
void display_set_rgb(uint8_t led, uint8_t r, uint8_t g, uint8_t b);

void display_set_fast_rgb(uint8_t start, uint8_t count, uint8_t* values);
void display_set_fast_hsv(uint8_t start, uint8_t count, uint8_t* values);

void display_clear(void);

void set_pattern_rgb(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b);
void set_pattern_rgb_ex(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint32_t period_ms, int32_t offset_ms);
void clear_pattern(uint8_t led);

void display_set_boot_pattern(void);

#endif // DISPLAY_H
