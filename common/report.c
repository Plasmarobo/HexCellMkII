#include "report.h"

#include "serial_output.h"

#include <stdint.h>

void report_error(report_type_t type, uint32_t error_code, uint32_t status, uint32_t context)
{
  // Ship a network message? NOOP for now
}