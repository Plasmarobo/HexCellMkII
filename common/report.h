#ifndef REPORT_H
#define REPORT_H

#include <stdint.h>

typedef enum
{
  RT_DEBUG = 0,
  RT_INFO,
  RT_WARNING,
  RT_ERROR,
} report_type_t;

void report_error(report_type_t type, uint32_t error_code, uint32_t status, uint32_t context);

#endif // REPORT_H