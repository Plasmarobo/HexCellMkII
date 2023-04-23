
#include "display.h"

#include "leds.h"
#include "time.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

//------------------------------------
// Macro
//------------------------------------

#define COLORS_PER_LED (3)

#define OTHER_INDEX (0)
#define TX_INDEX (1)

#define PATTERN_PERIOD_DEFAULT_MS (1000)
#define PATTERN_UPDATE_LIMIT_MS (20)

//------------------------------------
// Type Definition
//------------------------------------

typedef struct
{
  pattern_t pattern;
  uint32_t  period; // Pattern-defined period in ms
  uint32_t  offset; // Pattern offset in ms
  uint8_t   led_r;
  uint8_t   led_g;
  uint8_t   led_b;
} pattern_data_t;

typedef void (*pattern_engine_t)(uint8_t led, pattern_data_t* ref);

//------------------------------------
// Variable Declaration
//------------------------------------
#ifdef DISPLAY_ENABLED
static pattern_data_t pattern_data[LED_COUNT];
static uint32_t       pattern_timer;
#endif

//------------------------------------
// Private Functions
//------------------------------------

void solid_pattern_engine(uint8_t led, pattern_data_t* ref)
{
  set_rgb(led, ref->led_r, ref->led_g, ref->led_b);
}

void blink_pattern_engine(uint8_t led, pattern_data_t* ref)
{
  // Progress will be between 0 and period
  uint32_t progress = (get_milliseconds() + ref->offset) % (ref->period);
  // Get a value between 0 and 256
  uint8_t percentage = (256 * progress) / (ref->period);
  if (percentage < 128)
  {
    set_rgb(led, ref->led_r, ref->led_g, ref->led_b);
  }
  else
  {
    set_rgb(led, 0, 0, 0);
  }
}

//------------------------------------
// Public Functions
//------------------------------------

void display_init(void)
{
#ifdef DISPLAY_ENABLED
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    clear_pattern(led);
  }
  pattern_timer = get_milliseconds();
#endif
}

void display_update(opt_callback_t callback)
{
#ifdef DISPLAY_ENABLED
  if ((get_milliseconds() - pattern_timer) > PATTERN_UPDATE_LIMIT_MS)
  {
    for (uint8_t i = 0; i < LED_COUNT; ++i)
    {
      pattern_engine_t eng = NULL;
      switch (pattern_data[i].pattern)
      {
        case PATTERN_BLINK:
          eng = blink_pattern_engine;
          break;
        case PATTERN_SOLID: // Intentional fallthrough
        default:
          eng = solid_pattern_engine;
          break;
      }
      if (eng != NULL)
      {
        eng(i, &pattern_data[i]);
      }
    }
    update_leds(callback);
    pattern_timer = get_milliseconds();
  }
  else
  {
    if (callback != NULL)
    {
      callback(0, 0);
    }
  }
#endif
}

void display_set_hsv(uint8_t led, uint8_t h, uint8_t s, uint8_t v)
{
#ifdef DISPLAY_ENABLED
  set_hsv(led, h, s, v);
#endif
}

void display_set_rgb(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
#ifdef DISPLAY_ENABLED
  set_rgb(led, r, g, b);
#endif
}

void display_set_fast_rgb(uint8_t start, uint8_t count, uint8_t* values)
{
#ifdef DISPLAY_ENABLED
  for (uint8_t i = start; i < start + count; ++i)
  {
    set_rgb(i, *values, *(values + 1), *(values + 2));
    values += 3;
  }
#endif
}

void display_set_fast_hsv(uint8_t start, uint8_t count, uint8_t* values)
{
#ifdef DISPLAY_ENABLED
  for (uint8_t i = start; i < start + count; ++i)
  {
    set_hsv(i, *values, *(values + 1), *(values + 2));
    values += 3;
  }
#endif
}

void display_clear(void)
{
#ifdef DISPLAY_ENABLED
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    set_rgb(led, 0, 0, 0);
  }
#endif
}

void set_pattern_rgb(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
#ifdef DISPLAY_ENABLED
  if (led < LED_COUNT)
  {
    pattern_data[led].pattern = pattern;
    pattern_data[led].led_r   = r;
    pattern_data[led].led_g   = g;
    pattern_data[led].led_b   = b;
    pattern_data[led].offset  = 0;
    pattern_data[led].period  = PATTERN_PERIOD_DEFAULT_MS;
  }
#endif
}

void set_pattern_rgb_ex(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint32_t period_ms, int32_t offset_ms)
{
#ifdef DISPLAY_ENABLED
  set_pattern_rgb(pattern, led, r, g, b);
  pattern_data[led].period = period_ms;
  pattern_data[led].offset = offset_ms;
#endif
}

void clear_pattern(uint8_t led)
{
#ifdef DISPLAY_ENABLED
  if (led < LED_COUNT)
  {
    pattern_data[led].pattern = PATTERN_SOLID;
    pattern_data[led].offset  = 0;
    pattern_data[led].led_r   = 0;
    pattern_data[led].led_g   = 0;
    pattern_data[led].led_b   = 0;
    pattern_data[led].period  = PATTERN_PERIOD_DEFAULT_MS;
  }
#endif
}

void display_set_boot_pattern(void)
{
#ifdef DISPLAY_ENABLED
  uint8_t r = 0;
  uint8_t g = 128;
  uint8_t b = 0;

  for (int8_t i = 0; i < LED_COUNT; ++i)
  {
    set_pattern_rgb_ex(PATTERN_BLINK, i, r, g, b, 1000 * LED_COUNT, 1000 * i);
    uint8_t tmp = b;
    b           = g;
    g           = r;
    r           = tmp;
  }
#endif
}