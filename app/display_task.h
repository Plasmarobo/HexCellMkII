#ifndef DISPLAY_TASK_H
#define DISPLAY_TASK_H

#include <stdint.h>

void display_task_init(void);
void display_task_handler(void* argument);
void display_update_complete_handler(int32_t error, uint32_t userdata);

#endif // DISPLAY_TASK_H