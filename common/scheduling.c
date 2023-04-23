#include "scheduling.h"

#include "tim.h"

#include <stdint.h>
#include <string.h>

typedef struct
{
  task_ptr_t* task;
  uint64_t    delay;
} event_t;

static event_t events[MAX_EVENTS];
static uint8_t head;
static uint8_t tail;

void init_scheduling(void)
{
  memset(events, 0, sizeof(event_t) * MAX_EVENTS);
}

void start_scheduler(void)
{
}

void pump_scheduler(void)
{
}

void do_at(task_ptr_t task, uint64_t delay_us)
{
}
void do_next(task_ptr_t task)
{
}
