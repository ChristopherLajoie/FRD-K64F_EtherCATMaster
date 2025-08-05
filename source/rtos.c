#include "rtos.h"
#include "CANopen_HAL.h"
#include "UART_HAL.h"

/* Global task handles */
TaskHandle_t g_ethercat_task_handle = NULL;
TaskHandle_t g_can_task_handle = NULL;
TaskHandle_t g_logger_task_handle = NULL;
SemaphoreHandle_t g_can_data_mutex = NULL;

/* Shared control data */
shared_control_data_t g_control_data = {
    .x_axis = 0,
    .y_axis = 0,
    .enable = 0,
    .speed_mode = 0,
    .estop = 1,  // Default to NOT emergency stopped
    .horn = 0,
    .last_update_ms = 0
};

/* Thread-safe data access functions */
bool get_control_data_safe(shared_control_data_t *data)
{
    if (xSemaphoreTake(g_can_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        *data = g_control_data;
        xSemaphoreGive(g_can_data_mutex);
        return true;
    }
    return false;
}

void set_control_data_safe(const shared_control_data_t *data)
{
    if (xSemaphoreTake(g_can_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        g_control_data = *data;
        g_control_data.last_update_ms = xTaskGetTickCount();
        xSemaphoreGive(g_can_data_mutex);
    }
}
