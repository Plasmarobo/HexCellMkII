#ifndef SCHEDULING_H
#define SCHEDULING_H

#include <stdint.h>

#define MAX_EVENTS (8)

typedef void (*task_ptr_t)(void);

void init_scheduling(void);
void start_scheduler(void);
void pump_scheduler(void);

void do_at(task_ptr_t task, uint64_t delay_us);
void do_next(task_ptr_t task);

#endif // SCHEDULING_H