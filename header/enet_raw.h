/*
 * Raw Ethernet Layer for FRDM-K64F EtherCAT Implementation
 * SOEM-Compatible API for EtherCAT Frame Processing
 */

#ifndef ENET_RAW_H
#define ENET_RAW_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "fsl_enet.h"
#include "fsl_phy.h"
#include "FreeRTOS.h"
#include "semphr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* EtherCAT Protocol Constants */
#define ETHERCAT_ETHERTYPE      0x88A4
#define ETHERCAT_MAX_FRAME_SIZE 1518
#define ETHERCAT_MIN_FRAME_SIZE 64

/* Buffer Configuration */
#define ENET_RAW_RXBD_NUM       8
#define ENET_RAW_TXBD_NUM       8
#define ENET_RAW_BUFFER_SIZE    1536  /* Must accommodate max Ethernet frame */

/* Timeout Values */
#define ENET_RAW_TX_TIMEOUT_MS  10
#define ENET_RAW_RX_TIMEOUT_MS  1
#define ENET_RAW_LINK_TIMEOUT_S 10

/* Return Status Codes */
typedef enum {
    ENET_RAW_SUCCESS = 0,
    ENET_RAW_ERROR_INIT = -1,
    ENET_RAW_ERROR_TIMEOUT = -2,
    ENET_RAW_ERROR_NO_BUFFER = -3,
    ENET_RAW_ERROR_INVALID_PARAM = -4,
    ENET_RAW_ERROR_NO_LINK = -5,
    ENET_RAW_ERROR_FRAME_SIZE = -6
} enet_raw_status_t;

/* Frame Structure for Zero-Copy Access */
typedef struct {
    uint8_t *data;          /* Pointer to frame data (zero-copy) */
    uint16_t length;        /* Frame length in bytes */
    uint32_t timestamp;     /* Reception timestamp (if available) */
} enet_raw_frame_t;

/* Network Interface Statistics */
typedef struct {
    uint32_t tx_frames;     /* Transmitted frames */
    uint32_t rx_frames;     /* Received frames */
    uint32_t tx_errors;     /* Transmission errors */
    uint32_t rx_errors;     /* Reception errors */
    uint32_t rx_dropped;    /* Dropped frames (buffer full) */
    uint32_t non_ethercat;  /* Non-EtherCAT frames filtered */
} enet_raw_stats_t;

/* Network Interface Handle */
typedef struct {
    enet_handle_t enet_handle;
    phy_handle_t phy_handle;
    mdio_handle_t mdio_handle;

    /* Synchronization */
    SemaphoreHandle_t tx_mutex;
    SemaphoreHandle_t rx_semaphore;

    /* Statistics */
    enet_raw_stats_t stats;

    /* Configuration */
    uint8_t mac_addr[6];
    bool promiscuous_mode;
    bool link_up;

    /* Buffer Management */
    uint8_t current_rx_idx;
    uint8_t current_tx_idx;

} enet_raw_handle_t;

/*******************************************************************************
 * SOEM-Compatible API Functions
 ******************************************************************************/

/**
 * @brief Initialize raw Ethernet interface
 * @param handle Pointer to interface handle
 * @param mac_addr MAC address for interface (6 bytes)
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_init(enet_raw_handle_t *handle, const uint8_t *mac_addr);

/**
 * @brief Close raw Ethernet interface
 * @param handle Pointer to interface handle
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_close(enet_raw_handle_t *handle);

/**
 * @brief Send Ethernet frame (non-blocking)
 * @param handle Pointer to interface handle
 * @param frame Pointer to frame data
 * @param length Frame length in bytes
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_send_frame(enet_raw_handle_t *handle,
                                     const uint8_t *frame,
                                     uint16_t length);

/**
 * @brief Receive Ethernet frame (blocking with timeout)
 * @param handle Pointer to interface handle
 * @param frame Pointer to frame structure (zero-copy)
 * @param timeout_ms Timeout in milliseconds
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_receive_frame(enet_raw_handle_t *handle,
                                        enet_raw_frame_t *frame,
                                        uint32_t timeout_ms);

/**
 * @brief Release received frame buffer (for zero-copy)
 * @param handle Pointer to interface handle
 * @param frame Pointer to frame structure
 */
void enet_raw_release_frame(enet_raw_handle_t *handle, enet_raw_frame_t *frame);

/**
 * @brief Check if link is up
 * @param handle Pointer to interface handle
 * @return true if link is up, false otherwise
 */
bool enet_raw_is_link_up(enet_raw_handle_t *handle);

/**
 * @brief Get interface statistics
 * @param handle Pointer to interface handle
 * @param stats Pointer to statistics structure
 */
void enet_raw_get_stats(enet_raw_handle_t *handle, enet_raw_stats_t *stats);

/**
 * @brief Reset interface statistics
 * @param handle Pointer to interface handle
 */
void enet_raw_reset_stats(enet_raw_handle_t *handle);

/*******************************************************************************
 * Testing and Debug Functions
 ******************************************************************************/

/**
 * @brief Send test EtherCAT frame for testing without slaves
 * @param handle Pointer to interface handle
 * @param sequence_num Sequence number for frame identification
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_send_test_frame(enet_raw_handle_t *handle, uint16_t sequence_num);

/**
 * @brief Send broadcast ping frame
 * @param handle Pointer to interface handle
 * @return ENET_RAW_SUCCESS on success, error code otherwise
 */
enet_raw_status_t enet_raw_send_ping_frame(enet_raw_handle_t *handle);

/**
 * @brief Dump frame contents to debug console
 * @param frame Pointer to frame data
 * @param length Frame length
 * @param label Description label for output
 */
void enet_raw_dump_frame(const uint8_t *frame, uint16_t length, const char *label);

/*******************************************************************************
 * Utility Macros
 ******************************************************************************/

/* Extract EtherType from Ethernet frame */
#define ENET_RAW_GET_ETHERTYPE(frame) \
    ((uint16_t)(((frame)[12] << 8) | (frame)[13]))

/* Check if frame is EtherCAT */
#define ENET_RAW_IS_ETHERCAT(frame) \
    (ENET_RAW_GET_ETHERTYPE(frame) == ETHERCAT_ETHERTYPE)

/* Get source MAC from frame */
#define ENET_RAW_GET_SRC_MAC(frame) (&(frame)[6])

/* Get destination MAC from frame */
#define ENET_RAW_GET_DST_MAC(frame) (&(frame)[0])

/* Frame payload start (after Ethernet header) */
#define ENET_RAW_GET_PAYLOAD(frame) (&(frame)[14])

/* Frame payload length */
#define ENET_RAW_GET_PAYLOAD_LEN(total_len) ((total_len) - 14)

#endif /* ENET_RAW_H */
