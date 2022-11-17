#ifndef SERIAL_OUTPUT_H
#define SERIAL_OUTPUT_H
/**
 * @file serial_output.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * This file defines the interface fon serial communications
 * to a development host. BSP implements details. Non-dev builds will likely
 * no-op this interface.
 */
#include <stdarg.h>

/**
 * @brief Sends characters in debug mode
 *
 * @param str String to send
 * @return int Characters sent/copied
 */
int serial_print(const char* str);
/**
 * @brief Sends a formatted string in debug mode
 *
 * @param format formatstring
 * @param ... varargs
 * @return int Characters send/copied
 */
int serial_printf(const char* format, ...);

#endif // SERIAL_OUTPUT_H