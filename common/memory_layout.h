#ifndef MEMORY_LAYOUT_H
#define MEMORY_LAYOUT_H
/**
 * @file memory_layout.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Provides some symbols to allow upper layers to manage firmware updates
 */

#include <stdarg.h>
#include <stdint.h>

// Assigned from linker, or defined in the BSP
extern const uintptr_t ORIGIN_BL;
extern const uint32_t  LENGTH_BL;
extern const uintptr_t ORIGIN_BL_META;
extern const uintptr_t ORIGIN_APP;
extern const uint32_t  LENGTH_APP;
extern const uintptr_t ORIGIN_APP_META;
extern const uint32_t  LENGTH_META;

#endif // MEMORY_LAYOUT_H