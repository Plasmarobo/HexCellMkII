#ifndef __BOOT__H_
#define __BOOT__H_
/**
 * @file boot.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Helpful interfaces for booting to a region of memory
 */
#include <stdint.h>

typedef void (*boot_function_t)(void);

void boot_jump_to_image(uint32_t* vector_table);

#endif