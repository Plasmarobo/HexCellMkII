#ifndef _TIME_H_
#define _TIME_H_
/**
 * @file
 * @author Austen Danger Bartels
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * This layer defines an abstract way to get milli- and micro-second timing.
 * Each BSP implements the timing, it may be imperfect. Expect less than 1% accuracy
 * Not for use in RTC timing
 */

void     time_init(void);
uint32_t get_milliseconds(void);
uint64_t get_microseconds(void);

#endif // _TIME_H_