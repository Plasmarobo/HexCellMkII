#include "boot.h"

#include "stm32f0xx.h"

void boot_jump_to_image(uint32_t *p_vector_table)
{
  // Stack pointer is the first entry in the vector table
  uint32_t stack_pointer = *(p_vector_table);
  // Reset handler is the second entry in the vector table
  uint32_t reset_handler_address = (*(p_vector_table + 1));

  boot_function_t jump_function = (boot_function_t)(reset_handler_address);

  // Set stack pointer
  __set_MSP(stack_pointer);

  // Set program counter
  jump_function();
}