/*
 * Raw Ethernet Layer Implementation for FRDM-K64F EtherCAT
 * Zero-copy, high-performance Ethernet interface
 */

#include "enet_raw.h"
#include "fsl_enet_mdio.h"
#include "fsl_phyksz8081.h"
#include "fsl_sysmpu.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "Utilities.h"

/*******************************************************************************
 * Private Definitions
 ******************************************************************************/

/* ENET Hardware Configuration */
#define ENET_RAW_BASE           ENET
#define ENET_RAW_PHY_ADDRESS    0x00U
#define ENET_RAW_CLOCK_FREQ     CLOCK_GetFreq(kCLOCK_CoreSysClk)

/* MDIO and PHY Operations */
#define ENET_RAW_MDIO_OPS       enet_ops
#define ENET_RAW_PHY_OPS        phyksz8081_ops

/* Buffer Alignment Requirements */
#ifndef APP_ENET_BUFF_ALIGNMENT
#define APP_ENET_BUFF_ALIGNMENT ENET_BUFF_ALIGNMENT
#endif

/*******************************************************************************
 * Private Variables
 ******************************************************************************/

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    taskDISABLE_INTERRUPTS();
    UART_PRINTF("Stack overflow in task: %s\r\n", pcTaskName);
    for(;;);
}

/* DMA Buffer Descriptors (must be in non-cacheable memory) */
AT_NONCACHEABLE_SECTION_ALIGN(
    static enet_rx_bd_struct_t s_rxBuffDescrip[ENET_RAW_RXBD_NUM],
    ENET_BUFF_ALIGNMENT
);

AT_NONCACHEABLE_SECTION_ALIGN(
    static enet_tx_bd_struct_t s_txBuffDescrip[ENET_RAW_TXBD_NUM],
    ENET_BUFF_ALIGNMENT
);

/* DMA Data Buffers (non-cacheable for zero-copy) */
AT_NONCACHEABLE_SECTION_ALIGN(
    static uint8_t s_rxDataBuff[ENET_RAW_RXBD_NUM][SDK_SIZEALIGN(ENET_RAW_BUFFER_SIZE, APP_ENET_BUFF_ALIGNMENT)],
    APP_ENET_BUFF_ALIGNMENT
);

AT_NONCACHEABLE_SECTION_ALIGN(
    static uint8_t s_txDataBuff[ENET_RAW_TXBD_NUM][SDK_SIZEALIGN(ENET_RAW_BUFFER_SIZE, APP_ENET_BUFF_ALIGNMENT)],
    APP_ENET_BUFF_ALIGNMENT
);

/*******************************************************************************
 * Private Functions
 ******************************************************************************/

/**
 * @brief ENET interrupt callback for frame reception
 */
static void enet_raw_callback(ENET_Type *base, enet_handle_t *enet_handle,
                            enet_event_t event, enet_frame_info_t *frameInfo, void *userData)
{
    enet_raw_handle_t *handle = (enet_raw_handle_t *)userData;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch (event)
    {
        case kENET_RxEvent:
            /* Signal RX semaphore from ISR */
            xSemaphoreGiveFromISR(handle->rx_semaphore, &xHigherPriorityTaskWoken);
            handle->stats.rx_frames++;
            break;

        case kENET_TxEvent:
            /* TX complete - nothing special needed for non-blocking operation */
            break;

        case kENET_ErrEvent:
            handle->stats.rx_errors++;
            break;

        default:
            break;
    }

    /* Yield to higher priority task if needed */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief Check if received frame is EtherCAT
 */
static bool enet_raw_is_ethercat_frame(const uint8_t *frame, uint16_t length)
{
    if (length < 14) /* Minimum Ethernet header size */
    {
        return false;
    }

    return ENET_RAW_IS_ETHERCAT(frame);
}

/*******************************************************************************
 * Public API Implementation
 ******************************************************************************/

enet_raw_status_t enet_raw_init(enet_raw_handle_t *handle, const uint8_t *mac_addr)
{
    enet_config_t config;
    phy_config_t phyConfig = {0};
    bool link = false;
    bool autonego = false;
    phy_speed_t speed;
    phy_duplex_t duplex;
    status_t status;
    volatile uint32_t count = 0;

    if (!handle || !mac_addr)
    {
        UART_LOG("ENET Init: Invalid parameters\n");
        return ENET_RAW_ERROR_INVALID_PARAM;
    }

    SYSMPU_Enable(SYSMPU, false);

    handle->tx_mutex = xSemaphoreCreateMutex();
    handle->rx_semaphore = xSemaphoreCreateBinary();

    if (!handle->tx_mutex || !handle->rx_semaphore)
    {
        UART_LOG("ENET Init: Failed to create FreeRTOS synchronization objects\n");
        return ENET_RAW_ERROR_INIT;
    }

    enet_buffer_config_t buffConfig[] = {{
        ENET_RAW_RXBD_NUM,
        ENET_RAW_TXBD_NUM,
        SDK_SIZEALIGN(ENET_RAW_BUFFER_SIZE, APP_ENET_BUFF_ALIGNMENT),
        SDK_SIZEALIGN(ENET_RAW_BUFFER_SIZE, APP_ENET_BUFF_ALIGNMENT),
        &s_rxBuffDescrip[0],
        &s_txBuffDescrip[0],
        &s_rxDataBuff[0][0],
        &s_txDataBuff[0][0],
        true,
        true,
        NULL,
    }};

    ENET_GetDefaultConfig(&config);

    /* Set RMII mode - exactly like NXP */
    config.miiMode = kENET_RmiiMode;

    /* Configure PHY and MDIO - exactly like NXP example */
    phyConfig.phyAddr = 0x00U;
    phyConfig.autoNeg = true;
    handle->mdio_handle.ops = &enet_ops;
    handle->mdio_handle.resource.base = ENET;
    handle->mdio_handle.resource.csrClock_Hz = CLOCK_GetFreq(kCLOCK_CoreSysClk);

    handle->phy_handle.phyAddr = 0x00U;
    handle->phy_handle.mdioHandle = &handle->mdio_handle;
    handle->phy_handle.ops = &phyksz8081_ops;

    do
    {
        status = PHY_Init(&handle->phy_handle, &phyConfig);
        if (status == kStatus_Success)
        {
            /* Wait for auto-negotiation success and link up */
            count = 100000;  /* PHY_AUTONEGO_TIMEOUT_COUNT */
            do
            {
                PHY_GetAutoNegotiationStatus(&handle->phy_handle, &autonego);
                PHY_GetLinkStatus(&handle->phy_handle, &link);
                if (autonego && link)
                {
                    break;
                }
            } while (--count);
            if (!autonego)
            {
                UART_LOG("ENET Init: PHY Auto-negotiation failed. Please check the cable connection and link partner setting.\n");
            }
        }
    } while (!(link && autonego));

    UART_LOG("ENET Init: PHY initialization successful\n");

    /* Get the actual PHY link speed - exactly like NXP */
    PHY_GetLinkSpeedDuplex(&handle->phy_handle, &speed, &duplex);
    /* Change the MII speed and duplex for actual link status - exactly like NXP */
    config.miiSpeed = (enet_mii_speed_t)speed;
    config.miiDuplex = (enet_mii_duplex_t)duplex;
    handle->link_up = true;

    UART_PRINTF("ENET Init: Using actual link speed/duplex: %s/%s\r\n",
           (speed == kPHY_Speed100M) ? "100M" : "10M",
           (duplex == kPHY_FullDuplex) ? "Full" : "Half");

    /* Add EtherCAT-specific settings */
    config.rxMaxFrameLen = ETHERCAT_MAX_FRAME_SIZE;
    config.macSpecialConfig = kENET_ControlPromiscuousEnable; /* For EtherCAT */
    config.txAccelerConfig = 0;  /* Disable acceleration for raw access */
    config.rxAccelerConfig = 0;  /* Disable acceleration for raw access */
    config.pauseDuration = 0;    /* Disable flow control */

    ENET_Init(ENET, &handle->enet_handle, &config, &buffConfig[0],
              handle->mac_addr, CLOCK_GetFreq(kCLOCK_CoreSysClk));

    ENET_SetCallback(&handle->enet_handle, enet_raw_callback, handle);

    ENET_ActiveRead(ENET);

    /* Reset statistics */
    enet_raw_reset_stats(handle);

    UART_LOG("ENET Init: Initialization completed successfully!\n");
    return ENET_RAW_SUCCESS;
}

enet_raw_status_t enet_raw_close(enet_raw_handle_t *handle)
{
    if (!handle)
    {
        return ENET_RAW_ERROR_INVALID_PARAM;
    }

    /* Disable ENET peripheral */
    ENET_Deinit(ENET_RAW_BASE);

    /* Clean up FreeRTOS objects */
    if (handle->tx_mutex)
    {
        vSemaphoreDelete(handle->tx_mutex);
        handle->tx_mutex = NULL;
    }

    if (handle->rx_semaphore)
    {
        vSemaphoreDelete(handle->rx_semaphore);
        handle->rx_semaphore = NULL;
    }

    handle->link_up = false;

    return ENET_RAW_SUCCESS;
}

enet_raw_status_t enet_raw_send_frame(enet_raw_handle_t *handle,
                                     const uint8_t *frame,
                                     uint16_t length)
{
    status_t status;

    if (!handle || !frame || length < ETHERCAT_MIN_FRAME_SIZE ||
        length > ETHERCAT_MAX_FRAME_SIZE)
    {
        return ENET_RAW_ERROR_INVALID_PARAM;
    }

    if (!handle->link_up)
    {
        return ENET_RAW_ERROR_NO_LINK;
    }

    /* Take TX mutex with timeout */
    if (xSemaphoreTake(handle->tx_mutex, pdMS_TO_TICKS(ENET_RAW_TX_TIMEOUT_MS)) != pdTRUE)
    {
        return ENET_RAW_ERROR_TIMEOUT;
    }

    /* Send frame (non-blocking) */
    status = ENET_SendFrame(ENET_RAW_BASE, &handle->enet_handle, frame, length, 0, false, NULL);

    /* Release TX mutex */
    xSemaphoreGive(handle->tx_mutex);

    if (status == kStatus_Success)
    {
        handle->stats.tx_frames++;
        return ENET_RAW_SUCCESS;
    }
    else if (status == kStatus_ENET_TxFrameBusy)
    {
        return ENET_RAW_ERROR_NO_BUFFER;
    }
    else
    {
        handle->stats.tx_errors++;
        return ENET_RAW_ERROR_INIT;
    }
}

enet_raw_status_t enet_raw_receive_frame(enet_raw_handle_t *handle,
                                        enet_raw_frame_t *frame,
                                        uint32_t timeout_ms)
{
    status_t status;
    uint32_t length = 0;
    uint8_t *data_ptr;

    if (!handle || !frame)
    {
        return ENET_RAW_ERROR_INVALID_PARAM;
    }

    /* Wait for frame reception with timeout */
    if (xSemaphoreTake(handle->rx_semaphore, pdMS_TO_TICKS(timeout_ms)) != pdTRUE)
    {
        return ENET_RAW_ERROR_TIMEOUT;
    }

    /* Check for received frame */
    status = ENET_GetRxFrameSize(&handle->enet_handle, &length, 0);

    if (status == kStatus_ENET_RxFrameError)
    {
        /* Handle RX error - discard frame */
        ENET_ReadFrame(ENET_RAW_BASE, &handle->enet_handle, NULL, 0, 0, NULL);
        handle->stats.rx_errors++;
        return ENET_RAW_ERROR_INIT;
    }

    if (length == 0)
    {
        return ENET_RAW_ERROR_TIMEOUT;
    }

    /* Allocate temporary buffer for frame validation */
    data_ptr = pvPortMalloc(length);
    if (!data_ptr)
    {
        /* Release frame without reading */
        ENET_ReadFrame(ENET_RAW_BASE, &handle->enet_handle, NULL, 0, 0, NULL);
        return ENET_RAW_ERROR_NO_BUFFER;
    }

    /* Read frame data */
    status = ENET_ReadFrame(ENET_RAW_BASE, &handle->enet_handle, data_ptr, length, 0, NULL);

    if (status != kStatus_Success)
    {
        vPortFree(data_ptr);
        handle->stats.rx_errors++;
        return ENET_RAW_ERROR_INIT;
    }

    /* Filter for EtherCAT frames only */
    if (!enet_raw_is_ethercat_frame(data_ptr, length))
    {
        vPortFree(data_ptr);
        handle->stats.non_ethercat++;
        return ENET_RAW_ERROR_TIMEOUT; /* Try again for EtherCAT frame */
    }

    /* Fill frame structure (note: this is a copy, not zero-copy for now) */
    frame->data = data_ptr;
    frame->length = length;
    frame->timestamp = xTaskGetTickCount();

    return ENET_RAW_SUCCESS;
}

void enet_raw_release_frame(enet_raw_handle_t *handle, enet_raw_frame_t *frame)
{
    if (handle && frame && frame->data)
    {
        vPortFree(frame->data);
        frame->data = NULL;
        frame->length = 0;
    }
}

bool enet_raw_is_link_up(enet_raw_handle_t *handle)
{
    bool link = false;

    if (handle)
    {
        PHY_GetLinkStatus(&handle->phy_handle, &link);
        handle->link_up = link;
    }

    return link;
}

void enet_raw_get_stats(enet_raw_handle_t *handle, enet_raw_stats_t *stats)
{
    if (handle && stats)
    {
        *stats = handle->stats;
    }
}

void enet_raw_reset_stats(enet_raw_handle_t *handle)
{
    if (handle)
    {
        memset(&handle->stats, 0, sizeof(enet_raw_stats_t));
    }
}

/*******************************************************************************
 * Testing and Debug Functions
 ******************************************************************************/

enet_raw_status_t enet_raw_send_test_frame(enet_raw_handle_t *handle, uint16_t sequence_num)
{
    static uint8_t test_frame[64]; /* 64 bytes to meet ETHERCAT_MIN_FRAME_SIZE */
    uint32_t count = 0;

    if (!handle)
    {
        return ENET_RAW_ERROR_INVALID_PARAM;
    }

    /* Build EtherCAT test frame - similar to NXP BuildBroadCastFrame */
    /* Destination: Broadcast */
    for (count = 0; count < 6U; count++)
    {
        test_frame[count] = 0xFFU;
    }

    /* Source: Our MAC */
    memcpy(&test_frame[6], handle->mac_addr, 6U);

    /* EtherType: EtherCAT (0x88A4) instead of length */
    test_frame[12] = (ETHERCAT_ETHERTYPE >> 8) & 0xFFU;
    test_frame[13] = ETHERCAT_ETHERTYPE & 0xFFU;

    /* EtherCAT payload with test pattern */
    test_frame[14] = 0x01;  /* EtherCAT Length and Type */
    test_frame[15] = 0x10;  /* EtherCAT Info */
    test_frame[16] = (sequence_num >> 8) & 0xFF;  /* Sequence high */
    test_frame[17] = sequence_num & 0xFF;         /* Sequence low */

    /* Fill remaining payload with pattern like NXP example */
    for (count = 18; count < 60; count++)
    {
        test_frame[count] = (count - 18 + sequence_num) % 0xFFU;
    }

    /* Pad to 64 bytes (minimum EtherCAT frame size) */
    for (count = 60; count < 64; count++)
    {
        test_frame[count] = 0x00;
    }

    return enet_raw_send_frame(handle, test_frame, sizeof(test_frame));
}

enet_raw_status_t enet_raw_send_ping_frame(enet_raw_handle_t *handle)
{
    static uint16_t ping_sequence = 0;
    return enet_raw_send_test_frame(handle, ping_sequence++);
}

void enet_raw_dump_frame(const uint8_t *frame, uint16_t length, const char *label)
{
    if (!frame || length == 0)
    {
        return;
    }

    UART_PRINTF("\r\n=== %s (Length: %d) ===\r\n", label ? label : "Frame", length);

    /* Dump Ethernet header */
    UART_PRINTF("DST: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
           frame[0], frame[1], frame[2], frame[3], frame[4], frame[5]);
    UART_PRINTF("SRC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
           frame[6], frame[7], frame[8], frame[9], frame[10], frame[11]);
    UART_PRINTF("Type: 0x%04X", ENET_RAW_GET_ETHERTYPE(frame));

    if (ENET_RAW_IS_ETHERCAT(frame))
    {
        UART_LOG(" (EtherCAT)\n");
    }
    else
    {
        UART_LOG("\n");
    }

    /* Dump first 32 bytes of payload in hex */
    UART_LOG("Data: ");
    uint16_t dump_len = (length > 46) ? 32 : (length - 14);
    for (int i = 0; i < dump_len; i++)
    {
        if (i % 16 == 0) UART_LOG("\r\n  ");
        UART_PRINTF("%02X ", frame[14 + i]);
    }
    UART_LOG("\r\n");
}
