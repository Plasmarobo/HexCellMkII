#include "memory_layout.h"

#include <stdint.h>

extern const uintptr_t _ORIGIN_BL;
extern const uint32_t  _LENGTH_BL;
extern const uintptr_t _ORIGIN_BL_META;
extern const uintptr_t _ORIGIN_APP;
extern const uint32_t  _LENGTH_APP;
extern const uintptr_t _ORIGIN_APP_META;
extern const uint32_t  _LENGTH_META;

const uintptr_t ORIGIN_BL       = (uintptr_t)(&_ORIGIN_BL);
const uint32_t  LENGTH_BL       = (uint32_t)(&_LENGTH_BL);
const uintptr_t ORIGIN_BL_META  = (uintptr_t)(&_ORIGIN_BL_META);
const uintptr_t ORIGIN_APP      = (uintptr_t)(&_ORIGIN_APP);
const uint32_t  LENGTH_APP      = (uint32_t)(&_LENGTH_APP);
const uintptr_t ORIGIN_APP_META = (uintptr_t)(&_ORIGIN_APP_META);
const uint32_t  LENGTH_META     = (uint32_t)(&_LENGTH_META);