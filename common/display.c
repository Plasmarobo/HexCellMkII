
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
#define PATTERN_UPDATE_LIMIT_MS (5)

//------------------------------------
// Type Definition
//------------------------------------

typedef struct
{
  pattern_t pattern;  // Pattern
  uint32_t  timer;    // Pattern-defined timer in ticks
  uint32_t  period;   // Pattern-defined period in ticks
  uint8_t   led_r;
  uint8_t   led_g;
  uint8_t   led_b;
} pattern_data_t;

//------------------------------------
// Variable Declaration
//------------------------------------
#ifdef DISPLAY_ENABLED
static pattern_data_t patterns[LED_COUNT];
static uint32_t       limit_timer;
#endif

//------------------------------------
// Private Functions
//------------------------------------

void pattern_dispatch(uint8_t led_index, pattern_data_t* pattern_ref)
{
#ifdef DISPLAY_ENABLED
  if (NULL != pattern_ref)
  {
    // Shift into 255-0 space as a fixed-point (1.0-0.0) value
    uint32_t delta_ms = (get_milliseconds() - pattern_ref->timer);
    switch (pattern_ref->pattern)
    {
      case PATTERN_SOLID:
        // Transfer the color data
        display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
        break;
      case PATTERN_BREATHE:
        {
          uint8_t substate = (delta_ms * 8) / pattern_ref->period;
          switch (substate)
          {
            case 0:
              {
                // Fade in for 1 period
                uint8_t r, g, b;
                // Gives a value between 0-255 in periods
                r = (delta_ms * (uint32_t)pattern_ref->led_r) / pattern_ref->period;
                g = (delta_ms * (uint32_t)pattern_ref->led_g) / pattern_ref->period;
                b = (delta_ms * (uint32_t)pattern_ref->led_b) / pattern_ref->period;

                display_set_rgb(led_index, r, g, b);
                break;
              }
            case 1:
            case 2:
              // Hold for 2 periods
              display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
              break;
            case 3:
              {
                // Fade out for 1 period
                uint8_t r, g, b;
                // As delta_ms approaches period, scaling factor approaches 255
                uint32_t scaling_factor = (delta_ms << 8) / pattern_ref->period;
                r                       = (pattern_ref->led_r * scaling_factor) >> 8;
                g                       = (pattern_ref->led_g * scaling_factor) >> 8;
                b                       = (pattern_ref->led_b * scaling_factor) >> 8;
                display_set_rgb(led_index, r, g, b);
              }
              break;
            case 4:
              // Hold off for 1 period
              display_set_rgb(led_index, 0, 0, 0);
              break;
            default:
              pattern_ref->timer = get_milliseconds();
              break;
          }
        }
        break;
      case PATTERN_HEARTBEAT:
        {
          // Calculate the position in the period
          // ON - OFF - ON - OFF - Rest (4)
          uint8_t substate = (delta_ms * 8) / pattern_ref->period;
          switch (substate)
          {
            case 0:
            case 2:
              display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
              break;
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
              display_set_rgb(led_index, 0, 0, 0);
              break;
            default:
              pattern_ref->timer = get_milliseconds();
              break;
          }
        }
        break;
      case PATTERN_BLINK:
        {
          // ON - OFF
          if (delta_ms < (pattern_ref->period / 2))
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }

          if (delta_ms >= (pattern_ref->period))
          {
            pattern_ref->timer = get_milliseconds();
          }
        }
        break;
      case PATTERN_SOS:
        {
          // ---___--- Rest ()
          const uint32_t mask     = 0b0001010100111011101110010101;
          uint8_t        substate = (delta_ms * 32) / pattern_ref->period;
          if ((1 << substate) & mask)
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }

          if (substate >= 32)
          {
            pattern_ref->timer = get_milliseconds();
          }
        }
        break;
      case PATTERN_CHASE:
        {
          uint32_t period_per_led = pattern_ref->period / LED_COUNT;
          uint8_t  active_led     = delta_ms / period_per_led;
          if (led_index == active_led)
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }
          if (delta_ms >= pattern_ref->period)
          {
            pattern_ref->timer = get_milliseconds();
          }
        }
      case PATTERN_NONE: // Don't mess with set value
        break;
      default:
        display_set_rgb(led_index, 0, 0, 0);
        break;
    }
  }
#endif
}

void pattern_engine_run(void)
{
#ifdef DISPLAY_ENABLED
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    pattern_dispatch(led, &patterns[led]);
  }
#endif
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
  display_clear();
  limit_timer = get_milliseconds();
#endif
}

void display_update(opt_callback_t callback)
{
#ifdef DISPLAY_ENABLED
  pattern_engine_run();
  update_leds(callback);
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
    patterns[led].pattern = pattern;
    patterns[led].led_r   = r;
    patterns[led].led_g   = g;
    patterns[led].led_b   = b;
    patterns[led].timer  = get_milliseconds();
    patterns[led].period = PATTERN_PERIOD_DEFAULT_MS;
  }
#endif
}

void set_pattern_rgb_ex(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b, uint32_t period_ms, int32_t offset_ms)
{
#ifdef DISPLAY_ENABLED
  set_pattern_rgb(pattern, led, r, g, b);
  patterns[led].period = period_ms;
  patterns[led].timer  = get_milliseconds() + offset_ms;
#endif
}

void clear_pattern(uint8_t led)
{
#ifdef DISPLAY_ENABLED
  if (led < LED_COUNT)
  {
    patterns[led].pattern = PATTERN_NONE;
    patterns[led].timer   = get_milliseconds();
    patterns[led].led_r   = 0;
    patterns[led].led_g   = 0;
    patterns[led].led_b   = 0;
    patterns[led].period  = PATTERN_PERIOD_DEFAULT_MS;
  }
#endif
}

void display_set_boot_pattern(void)
{
#ifdef DISPLAY_ENABLED
  uint8_t r = 0;
  uint8_t g = 128;
  uint8_t b = 0;

  for (uint8_t i = 0; i < LED_COUNT; ++i)
  {
    set_pattern_rgb_ex(PATTERN_SOLID, i, r, g, b, 1000, 250 * i);
    uint8_t tmp = b;
    b           = g;
    g           = r;
    r           = tmp;
  }
#endif
}