#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "INIT_HAL.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Utilities.h"
#include "rtos.h"
#include "enet_raw.h"  // Add this include

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    UART_LOG("FATAL: FreeRTOS malloc failed\r\n");
    for(;;);
}

// Forward declarations for test tasks
void ethernet_test_main_task(void *pvParameters);
void ethernet_test_rx_task(void *pvParameters);
void simple_logger_task(void *pvParameters);

int main(void)
{
    Init();
    UART_LogSetEnabled(true);

    #ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
    #endif

    g_can_data_mutex = xSemaphoreCreateMutex();
    if (g_can_data_mutex == NULL) {
        UART_LOG("Failed to create CAN data mutex\r\n");
        while(1);
    }

    BaseType_t result;

    // Create Ethernet test main task (replaces ethercat_task for now)
    result = xTaskCreate(
        ethernet_test_main_task,        /* Task function */
        "EthTest",                      /* Task name */
        ETHERCAT_TASK_STACK_SIZE,      /* Stack size (reuse your define) */
        NULL,                          /* Parameters */
        ETHERCAT_TASK_PRIORITY,        /* Priority (reuse your define) */
        &g_ethercat_task_handle        /* Handle */
    );
    if (result != pdPASS) {
        UART_LogMessage("Failed to create Ethernet test task\r\n");
        while(1);
    }

    // Skip CAN task for now (comment out)
    /*
    result = xTaskCreate(
        can_monitor_task,
        "CANMon",
        CAN_TASK_STACK_SIZE,
        NULL,
        CAN_MONITOR_TASK_PRIORITY,
        &g_can_task_handle
    );
    if (result != pdPASS) {
        UART_LogMessage("Failed to create CAN task\n");
        while(1);
    }
    */

    // Create simple logger task
    result = xTaskCreate(
        simple_logger_task,
        "Logger",
        LOGGER_TASK_STACK_SIZE,
        NULL,
        LOGGER_TASK_PRIORITY,
        &g_logger_task_handle
    );
    if (result != pdPASS) {
        UART_LogMessage("Failed to create Logger task\r\n");
        while(1);
    }

    vTaskStartScheduler();

    /* Should never get here */
    UART_LogMessage("FreeRTOS scheduler failed!\r\n");
    volatile static int i = 0;

    while(1) {
        i++;
        __asm volatile ("nop");
    }
    return 0;
}

/*******************************************************************************
 * Task Implementations
 ******************************************************************************/

// Simple logger task that just calls UART_LogProcess
void simple_logger_task(void *pvParameters)
{
    UART_LOG("Logger task started\r\n");

    while(1)
    {
        // Call your UART log processing function
        UART_LogProcess();

        // Run every 10ms as specified in your plan
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Ethernet test RX task (adapted from my original)
void ethernet_test_rx_task(void *pvParameters)
{
    enet_raw_handle_t *handle = (enet_raw_handle_t *)pvParameters;
    enet_raw_frame_t rx_frame;
    enet_raw_status_t status;
    uint32_t frame_count = 0;

    UART_LOG("Ethernet RX Task: Started\r\n");

    while (1)
    {
        // Try to receive EtherCAT frames with 100ms timeout
        status = enet_raw_receive_frame(handle, &rx_frame, 100);

        if (status == ENET_RAW_SUCCESS)
        {
            frame_count++;

            UART_PRINTF("RX[%lu]: Frame received, length=%d\r\n",
                       frame_count, rx_frame.length);

            // Check if this is our test frame by looking at sequence number
            if (rx_frame.length >= 18)
            {
                uint16_t sequence = (rx_frame.data[16] << 8) | rx_frame.data[17];
                UART_PRINTF("  Test frame sequence: %d\r\n", sequence);
            }

            // Release frame buffer
            enet_raw_release_frame(handle, &rx_frame);
        }
        else if (status != ENET_RAW_ERROR_TIMEOUT)
        {
            UART_PRINTF("RX Task: Receive error %d\r\n", status);
        }

        // Small delay to prevent overwhelming the console
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Main Ethernet test task (adapted from my original)
void ethernet_test_main_task(void *pvParameters)
{
    static enet_raw_handle_t s_enet_handle;  // Static to persist
    enet_raw_status_t status;
    enet_raw_stats_t stats;
    uint32_t ping_count = 0;
    uint32_t last_stats_time = 0;
    uint32_t last_ping_time = 0;
    uint32_t current_time;
    uint8_t test_mac[] = {0x02, 0x12, 0x13, 0x10, 0x15, 0x11};  // Test MAC

    const uint32_t PING_INTERVAL_MS = 1000;   // 1 second between pings
    const uint32_t STATS_INTERVAL_MS = 5000;  // 5 seconds between stats
    const uint32_t MAX_PING_COUNT = 20;       // Run 20 pings then repeat

    UART_LOG("\n=== FRDM-K64F Ethernet Layer Test ===\r\n");

    // Initialize raw Ethernet interface
	UART_LOG("Initializing Ethernet interface...\r\n");
	status = enet_raw_init(&s_enet_handle, test_mac);

	if (status != ENET_RAW_SUCCESS)
	{
		UART_PRINTF("ERROR: Failed to initialize Ethernet interface: %d\r\n", status);
	}

	UART_LOG("SUCCESS: Ethernet interface initialized\r\n");
	UART_PRINTF("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
			   test_mac[0], test_mac[1], test_mac[2],
			   test_mac[3], test_mac[4], test_mac[5]);

    // Check link status
    if (!enet_raw_is_link_up(&s_enet_handle))
    {
        UART_LOG("WARNING: Ethernet link is down\r\n");
        UART_LOG("Please check cable connection\r\n");
    }
    else
    {
        UART_LOG("SUCCESS: Ethernet link is up\r\n");
    }

    // Start RX task
    if (xTaskCreate(ethernet_test_rx_task, "EthRX", 2048, &s_enet_handle,
                    ETHERCAT_TASK_PRIORITY + 1, NULL) != pdPASS)
    {
        UART_LOG("ERROR: Failed to create RX test task\r\n");
        goto test_cleanup;
    }

    UART_LOG("\nStarting test loop...\n");
    UART_PRINTF("- Sending test EtherCAT frames every %lu ms\r\n", PING_INTERVAL_MS);
    UART_PRINTF("- Displaying statistics every %lu ms\r\n", STATS_INTERVAL_MS);

    last_stats_time = last_ping_time = xTaskGetTickCount();

    // Main test loop
    while (1)
    {
        current_time = xTaskGetTickCount();

        // Send test ping frames at regular intervals
        if ((current_time - last_ping_time) >= pdMS_TO_TICKS(PING_INTERVAL_MS))
        {
            status = enet_raw_send_test_frame(&s_enet_handle, ping_count);

            if (status == ENET_RAW_SUCCESS)
            {
                UART_PRINTF("TX[%lu]: Test frame sent (seq=%lu)\r\n",
                           ping_count + 1, ping_count);
            }
            else
            {
                UART_PRINTF("TX[%lu]: Send failed: %d\r\n", ping_count + 1, status);
            }

            ping_count++;
            if (ping_count >= MAX_PING_COUNT)
            {
                ping_count = 0;  // Reset and continue
            }

            last_ping_time = current_time;
        }

        // Display statistics periodically
        if ((current_time - last_stats_time) >= pdMS_TO_TICKS(STATS_INTERVAL_MS))
        {
            enet_raw_get_stats(&s_enet_handle, &stats);

            UART_LOG("\n--- Statistics ---\n");
            UART_PRINTF("TX Frames:    %lu\r\n", stats.tx_frames);
            UART_PRINTF("RX Frames:    %lu\r\n", stats.rx_frames);
            UART_PRINTF("TX Errors:    %lu\r\n", stats.tx_errors);
            UART_PRINTF("RX Errors:    %lu\r\n", stats.rx_errors);
            UART_PRINTF("RX Dropped:   %lu\r\n", stats.rx_dropped);
            UART_PRINTF("Non-EtherCAT: %lu\r\n", stats.non_ethercat);
            UART_PRINTF("Link Status:  %s\r\n",
                       enet_raw_is_link_up(&s_enet_handle) ? "UP" : "DOWN");
            UART_LOG("------------------\n");

            last_stats_time = current_time;
        }

        // Small delay
        vTaskDelay(pdMS_TO_TICKS(100));
    }

test_cleanup:
    // Close Ethernet interface
    enet_raw_close(&s_enet_handle);
    UART_LOG("Ethernet interface closed\r\n");

test_exit:
    UART_LOG("Ethernet test finished. Task will idle.\r\n");

    // Task completed - just idle
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
