#include "display_task.h"

#include "FreeRTOS.h"

#include "semphr.h"
#include "task.h"
#include "timers.h"

#include "display.h"
#include "priorities.h"

#define DISPLAY_MAX_WAIT_MS (10)
#define DISPLAY_UPDATE_MS (5)

#define DISPLAY_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE)

static StackType_t  stack_buffer[DISPLAY_TASK_STACK_SIZE];
static StaticTask_t tcb_buffer;
static TaskHandle_t display_task;

static SemaphoreHandle_t display_semaphore;
static StaticSemaphore_t display_mutex;

void display_task_init(void)
{
  display_semaphore = xSemaphoreCreateMutexStatic(&display_mutex);

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
  for (;;)
  {
    xSemaphoreTake(display_semaphore, portMAX_DELAY);
    display_update(display_update_complete_handler);
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    xSemaphoreGive(display_semaphore);
    vTaskDelay(pdMS_TO_TICKS(DISPLAY_UPDATE_MS));
  }
}
void display_update_complete_handler(int32_t error, uintptr_t userdata)
{
  // Possibly called from ISR
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  xTaskNotifyFromISR(display_task, TX_INDEX, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

bool display_lock(void)
{
  return pdTRUE == xSemaphoreTake(display_semaphore, pdMS_TO_TICKS(DISPLAY_MAX_WAIT_MS));
}

void display_unlock(void)
{
  xSemaphoreGive(display_semaphore);
}