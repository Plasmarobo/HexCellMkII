#ifndef OPT_PROTOTYPES_H
#define OPT_PROTOTYPES_H
/**
 * @file opt_prototypes.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Provides useful prototypes for callbacks other utility operations
 */
#include <stdint.h>

/**
 * @brief A callback that accepts and error code and address (pointer to userdata)
 * @param status
 * @param userdata
 */
typedef void (*opt_callback_t)(int32_t, uint32_t);

#endif // OPT_PROTOTYPES_H
