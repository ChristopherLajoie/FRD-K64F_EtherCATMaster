#ifndef TASKS_H
#define TASKS_H

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* Task priorities (higher number = higher priority) */
#define ETHERCAT_TASK_PRIORITY      (3)
#define CAN_MONITOR_TASK_PRIORITY   (2)
#define LOGGER_TASK_PRIORITY        (1)

/* Task stack sizes (in words, not bytes) */
#define ETHERCAT_TASK_STACK_SIZE    (4096 / sizeof(StackType_t))
#define CAN_TASK_STACK_SIZE         (2048 / sizeof(StackType_t))
#define LOGGER_TASK_STACK_SIZE      (2048 / sizeof(StackType_t))

/* Task periods */
#define ETHERCAT_PERIOD_MS          (4)    // 4ms cycle time
#define CAN_POLL_PERIOD_MS          (50)   // 50ms polling
#define LOGGER_PERIOD_MS            (10)   // 10ms log processing

/* Global handles */
extern TaskHandle_t g_ethercat_task_handle;
extern TaskHandle_t g_can_task_handle;
extern TaskHandle_t g_logger_task_handle;
extern SemaphoreHandle_t g_can_data_mutex;

/* Shared control data structure */
typedef struct {
    int16_t x_axis;
    int16_t y_axis;
    uint8_t enable;
    uint8_t speed_mode;
    uint8_t estop;
    uint8_t horn;
    uint32_t last_update_ms;
} shared_control_data_t;

extern shared_control_data_t g_control_data;

/* Task functions */
void ethercat_task(void *pvParameters);
void can_monitor_task(void *pvParameters);
void logger_task(void *pvParameters);

/* Utility functions */
_Bool get_control_data_safe(shared_control_data_t *data);
void set_control_data_safe(const shared_control_data_t *data);

#endif /* TASKS_H */
