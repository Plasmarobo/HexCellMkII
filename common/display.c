
#include "display.h"

#include "leds.h"
#include "time.h"

#include <stdint.h>
#include <string.h>

//------------------------------------
// Macro
//------------------------------------

#define COLORS_PER_LED (3)

#define OTHER_INDEX (0)
#define TX_INDEX (1)

#define PATTERN_PERIOD_DEFAULT_MS (1000)

//------------------------------------
// Type Definition
//------------------------------------

typedef struct
{
  pattern_t pattern;  // Pattern
  uint32_t  substate; // Pattern-defined substate
  uint32_t  timer;    // Pattern-defined timer in ticks
  uint32_t  period;   // Pattern-defined period in ticks
  uint8_t   led_r;
  uint8_t   led_g;
  uint8_t   led_b;
} pattern_data_t;

//------------------------------------
// Variable Declaration
//------------------------------------

static pattern_data_t patterns[LED_COUNT];

//------------------------------------
// Private Functions
//------------------------------------

void pattern_dispatch(uint8_t led_index, pattern_data_t* pattern_ref)
{
  uint32_t time = get_milliseconds();
  if (NULL != pattern_ref)
  {
    uint32_t scale_factor = ((time - pattern_ref->timer) << 8) / pattern_ref->period;
    switch (pattern_ref->pattern)
    {
      case PATTERN_SOLID:
        // Transfer the color data
        display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
        break;
      case PATTERN_BREATHE:
        {
          switch (pattern_ref->substate)
          {
            case 0:
              {
                // Fade in
                uint8_t r, g, b;
                // Gives a value between 0-255 in periods
                r = (scale_factor * pattern_ref->led_r) >> 8;
                g = (scale_factor * pattern_ref->led_g) >> 8;
                b = (scale_factor * pattern_ref->led_b) >> 8;

                if (scale_factor >= 255)
                {
                  // Advance to hold
                  pattern_ref->substate = 1;
                  pattern_ref->timer    = time;
                }
                else
                {
                  display_set_rgb(led_index, r, g, b);
                }
                break;
              }
            case 1:
              // Hold for 2 period
              display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
              if (scale_factor >= 512)
              {
                pattern_ref->substate = 2;
                pattern_ref->timer    = time;
              }
              break;
            case 2:
              {
                // Fade out
                uint8_t r, g, b;
                r = (pattern_ref->led_r * (255 - scale_factor)) >> 8;
                g = (pattern_ref->led_g * (255 - scale_factor)) >> 8;
                b = (pattern_ref->led_b * (255 - scale_factor)) >> 8;

                if (scale_factor >= 255)
                {
                  pattern_ref->substate = 3;
                  pattern_ref->timer    = time;
                }
                else
                {
                  display_set_rgb(led_index, r, g, b);
                }
              }
              break;
            case 3:
              display_set_rgb(led_index, 0, 0, 0);
              if (scale_factor >= 255)
              {
                pattern_ref->substate = 0;
                pattern_ref->timer    = time;
              }
              break;
            default:
              pattern_ref->substate = 0;
              pattern_ref->timer    = time;
              break;
          }
        }
        break;
      case PATTERN_HEARTBEAT:
        {
          // Calculate the position in the period
          // ON - OFF - ON - OFF - Rest (4)
          if ((0 == pattern_ref->substate) || (2 == pattern_ref->substate))
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }

          if (scale_factor >= 32)
          {
            ++pattern_ref->substate;
            pattern_ref->timer = time;
            if (7 <= pattern_ref->substate)
            {
              pattern_ref->substate = 0;
            }
          }
        }
        break;
      case PATTERN_BLINK:
        {
          // ON - OFF
          if ((1 == pattern_ref->substate))
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }

          if (scale_factor >= 64)
          {
            ++pattern_ref->substate;
            pattern_ref->timer = time;
            if (pattern_ref->substate > 1)
            {
              pattern_ref->substate = 0;
            }
          }
        }
        break;
      case PATTERN_SOS:
        {
          // ---___--- Rest ()
          const uint32_t mask = 0b0001010100111011101110010101;
          if ((1 << pattern_ref->substate) & mask)
          {
            display_set_rgb(led_index, pattern_ref->led_r, pattern_ref->led_g, pattern_ref->led_b);
          }
          else
          {
            display_set_rgb(led_index, 0, 0, 0);
          }

          if (scale_factor >= 32)
          {
            ++pattern_ref->substate;
            pattern_ref->timer = time;
            if (pattern_ref->substate > 31)
            {
              pattern_ref->substate = 0;
            }
          }
        }
        break;
      case PATTERN_NONE: // Don't mess with
        break;
      default:
        display_set_rgb(led_index, 0, 0, 0);
        break;
    }
  }
}

void pattern_engine_run(void)
{
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    pattern_dispatch(led, &patterns[led]);
  }
}

//------------------------------------
// Public Functions
//------------------------------------

void display_init(void)
{
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    clear_pattern(led);
  }
}

void display_update(opt_callback_t callback)
{
  pattern_engine_run();
  update_leds(callback);
}

void display_set_hsv(uint8_t led, uint8_t h, uint8_t s, uint8_t v)
{
  set_hsv(led, h, s, v);
}

void display_set_rgb(uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
  set_rgb(led, r, g, b);
}

void display_set_fast_rgb(uint8_t start, uint8_t count, uint8_t* values)
{
  for (uint8_t i = start; i < start + count; ++i)
  {
    set_rgb(i, *values, *(values + 1), *(values + 2));
    values += 3;
  }
}

void display_set_fast_hsv(uint8_t start, uint8_t count, uint8_t* values)
{
  for (uint8_t i = start; i < start + count; ++i)
  {
    set_hsv(i, *values, *(values + 1), *(values + 2));
    values += 3;
  }
}

void display_clear(void)
{
  for (uint8_t led = 0; led < LED_COUNT; ++led)
  {
    set_rgb(led, 0, 0, 0);
  }
}

void set_pattern_rgb(pattern_t pattern, uint8_t led, uint8_t r, uint8_t g, uint8_t b)
{
  if (led < LED_COUNT)
  {
    patterns[led].pattern = pattern;
    patterns[led].led_r   = r;
    patterns[led].led_g   = g;
    patterns[led].led_b   = b;
    // Only used by breathe for now
    patterns[led].timer  = get_milliseconds();
    patterns[led].period = PATTERN_PERIOD_DEFAULT_MS;
  }
}

void clear_pattern(uint8_t led)
{
  if (led < LED_COUNT)
  {
    patterns[led].pattern = PATTERN_NONE;
  }
}