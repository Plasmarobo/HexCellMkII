#include "display_task.h"

#include "FreeRTOS.h"

#include "task.h"
#include "timers.h"

#include "display.h"
#include "priorities.h"
#include "time.h"

#define DISPLAY_MAX_WAIT_MS (100)
#define DISPLAY_UPDATE_MS (30)

#define DISPLAY_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

static StackType_t  stack_buffer[DISPLAY_TASK_STACK_SIZE];
static StaticTask_t tcb_buffer;
static TaskHandle_t display_task;

void display_task_init(void)
{
  display_task = xTaskCreateStatic(display_task_handler,
                                   "display_task",
                                   DISPLAY_TASK_STACK_SIZE,
                                   NULL,
                                   DISPLAY_TASK_PRIORITY,
                                   stack_buffer,
                                   &tcb_buffer);
}

void display_task_handler(void* argument)
{
  display_set_boot_pattern();
  for (;;)
  {
    uint32_t update_timer = get_milliseconds();
    display_update(display_update_complete_handler);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    uint32_t delta_time = get_milliseconds() - update_timer;
    if (delta_time < DISPLAY_UPDATE_MS)
    {
      vTaskDelay(pdMS_TO_TICKS(DISPLAY_UPDATE_MS - delta_time));
    }
  }
}

void display_update_complete_handler(int32_t error, uint32_t userdata)
{
  // Possibly called from ISR
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xTaskNotifyFromISR(display_task, 0, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
