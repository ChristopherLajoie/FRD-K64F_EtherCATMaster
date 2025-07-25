#include "CANopen_HAL.h"
#include "string.h"
#include "stdio.h"
#include "UART_HAL.h"
#include "Utilities.h"
#include "fsl_clock.h"
#include "peripherals.h"

#define SIZE_OF_AXE_X   256.0
#define SIZE_OF_AXE_Y   256.0

#define SCALE_OF_AXE_X  88.0
#define SCALE_OF_AXE_Y  88.0
#define DECALAGE_PWM    6.0

/* Define receive frame buffers */
flexcan_frame_t movesRecvFrame;
flexcan_frame_t ledRecvFrame;
flexcan_frame_t buttonsRecvFrame;

// Define variables that contain data received
Moves moves = {50.0, 50.0};
Led led = {};
Buttons buttons = {};

/*FUNCTION**********************************************************************
 *
 * Function Name :  SendCANData
 * Description   :  This function take a mailbox, an ID and a data array and send the message
 *                  to the CAN bus
 *
 *END**************************************************************************/
void SendCANData(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len)
{
    flexcan_frame_t txFrame;

    // Clear the frame structure
    memset(&txFrame, 0, sizeof(flexcan_frame_t));

    // Set frame properties
    txFrame.id = FLEXCAN_ID_STD(messageId);  // Standard frame format
    txFrame.format = kFLEXCAN_FrameFormatStandard;
    txFrame.type = kFLEXCAN_FrameTypeData;
    txFrame.length = len;

    // Copy data to frame (up to 8 bytes)
    if (len > 0 && len <= 8) {
        for (uint32_t i = 0; i < len; i++) {
            if (i < 4) {
                txFrame.dataWord0 |= ((uint32_t)data[3-i] << (i * 8));
            } else {
                txFrame.dataWord1 |= ((uint32_t)data[7-(i-4)] << ((i-4) * 8));
            }
        }
    }

    /* Configure TX message buffer if not already configured */
    FLEXCAN_SetTxMbConfig(FLEXCAN_INSTANCE, mailbox, true);

    /* Send the frame */
    FLEXCAN_WriteTxMb(FLEXCAN_INSTANCE, mailbox, &txFrame);
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  FLEXCAN_Receive_Moves
 * Description   :  This function configures the data buffer and starts the receiving process
 *                  of the information at 0x28A ID. It waits until the receiving process
 *                  is done and update the moves variable that contains the values for both controller joysticks
 *
 *END**************************************************************************/
status_t FLEXCAN_Receive_Moves(void)
{
    uint32_t flag = 1U << RX_MOVES_MAILBOX;
    uint32_t timeout = 1000;  // Timeout counter

    /* Wait until message is received or timeout */
    while ((FLEXCAN_GetMbStatusFlags(FLEXCAN_INSTANCE, flag) == 0) && (timeout > 0)) {
        timeout--;
        // Small delay to prevent busy waiting
        for (volatile int i = 0; i < 1000; i++);
    }

    if (timeout == 0) {
        return kStatus_Timeout;
    }

    /* Read the received message */
    status_t result = FLEXCAN_ReadRxMb(FLEXCAN_INSTANCE, RX_MOVES_MAILBOX, &movesRecvFrame);

    /* Clear the flag */
    FLEXCAN_ClearMbStatusFlags(FLEXCAN_INSTANCE, flag);

    if (result == kStatus_Success || result == kStatus_FLEXCAN_RxOverflow) {
        // Update moves structure from received data
        // Assuming the data format is preserved from original implementation
        uint8_t* dataBytes = (uint8_t*)&movesRecvFrame.dataWord0;
        moves.Axe_X = (float)((dataBytes[0] << 8) | dataBytes[1]) / 100.0f;
        moves.Axe_Y = (float)((dataBytes[2] << 8) | dataBytes[3]) / 100.0f;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  FLEXCAN_Receive_Buttons
 * Description   :  This function configures the data buffer and starts the receiving process
 *                  of the information at 0x18A ID. It waits until the receiving process
 *                  is done and update the buttons variable that contains the values for the
 *                  enable, speed, starts, e-stop and horn.
 *
 *END**************************************************************************/
status_t FLEXCAN_Receive_Buttons(void)
{
    uint32_t flag = 1U << RX_BUTTONS_MAILBOX;
    uint32_t timeout = 1000;  // Timeout counter

    /* Wait until message is received or timeout */
    while ((FLEXCAN_GetMbStatusFlags(FLEXCAN_INSTANCE, flag) == 0) && (timeout > 0)) {
        timeout--;
        // Small delay to prevent busy waiting
        for (volatile int i = 0; i < 1000; i++);
    }

    if (timeout == 0) {
        return kStatus_Timeout;
    }

    /* Read the received message */
    status_t result = FLEXCAN_ReadRxMb(FLEXCAN_INSTANCE, RX_BUTTONS_MAILBOX, &buttonsRecvFrame);

    /* Clear the flag */
    FLEXCAN_ClearMbStatusFlags(FLEXCAN_INSTANCE, flag);

    if (result == kStatus_Success || result == kStatus_FLEXCAN_RxOverflow) {
        // Update buttons structure from received data
        uint8_t* dataBytes = (uint8_t*)&buttonsRecvFrame.dataWord0;
        buttons.Enable = (dataBytes[0] & 0x01) ? true : false;
        buttons.speed = (dataBytes[0] & 0x02) ? true : false;
        buttons.E_Stop = (dataBytes[0] & 0x04) ? true : false;
        buttons.Horn = (dataBytes[0] & 0x08) ? true : false;
        buttons.CAN_Enable = (dataBytes[0] & 0x10) ? true : false;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  FLEXCAN_Receive_Led
 * Description   :  This function configures the data buffer and starts the receiving process
 *                  of the information at 0x30A ID. It waits until the receiving process
 *                  is done and update the Led variable that contains the values for all controler LEDs
 *
 *END**************************************************************************/
status_t FLEXCAN_Receive_Led(void)
{
    uint32_t flag = 1U << RX_LED_MAILBOX;
    uint32_t timeout = 1000;  // Timeout counter

    /* Wait until message is received or timeout */
    while ((FLEXCAN_GetMbStatusFlags(FLEXCAN_INSTANCE, flag) == 0) && (timeout > 0)) {
        timeout--;
        // Small delay to prevent busy waiting
        for (volatile int i = 0; i < 1000; i++);
    }

    if (timeout == 0) {
        return kStatus_Timeout;
    }

    /* Read the received message */
    status_t result = FLEXCAN_ReadRxMb(FLEXCAN_INSTANCE, RX_LED_MAILBOX, &ledRecvFrame);

    /* Clear the flag */
    FLEXCAN_ClearMbStatusFlags(FLEXCAN_INSTANCE, flag);

    if (result == kStatus_Success || result == kStatus_FLEXCAN_RxOverflow) {
        // Update LED structure from received data
        uint8_t* dataBytes = (uint8_t*)&ledRecvFrame.dataWord0;
        led.Yellow_Bat_Led = (dataBytes[0] & 0x01) ? true : false;
        led.Red_Bat_Led = (dataBytes[0] & 0x02) ? true : false;
        led.Overload_Led = (dataBytes[0] & 0x04) ? true : false;
        led.Aux_Led = (dataBytes[0] & 0x08) ? true : false;
    }

    return result;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  Show_movesRecvBuffer
 * Description   :  This function is a simple debugging function that outputs the value
 *                  of both Joysticks on the UART
 *
 *END**************************************************************************/
void Show_movesRecvBuffer(void)
{
    char str[64];
    uint8_t* dataBytes = (uint8_t*)&movesRecvFrame.dataWord0;
    sprintf(str,"MOVES: %d %d %d %d %d %d %d %d\n",
            dataBytes[0], dataBytes[1], dataBytes[2], dataBytes[3],
            dataBytes[4], dataBytes[5], dataBytes[6], dataBytes[7]);
    UART_LOG(str);
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  Show_buttonsRecvBuffer
 * Description   :  This function is a simple debugging function that outputs the containt
 *                  of the button receiving buffer
 *
 *END**************************************************************************/
void Show_buttonsRecvBuffer(void)
{
    char str[64];
    uint8_t* dataBytes = (uint8_t*)&buttonsRecvFrame.dataWord0;
    sprintf(str,"BUTTONS: %d %d %d %d %d %d %d %d\n",
            dataBytes[0], dataBytes[1], dataBytes[2], dataBytes[3],
            dataBytes[4], dataBytes[5], dataBytes[6], dataBytes[7]);
    UART_LOG(str);
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  Show_ledRecvBuffer
 * Description   :  This function is a simple debugging function that outputs the containt
 *                  of the Led receiving buffer
 *
 *END**************************************************************************/
void Show_ledRecvBuffer(void)
{
    char str[64];
    uint8_t* dataBytes = (uint8_t*)&ledRecvFrame.dataWord0;
    sprintf(str,"LED: %d %d %d %d %d %d %d %d\n",
            dataBytes[0], dataBytes[1], dataBytes[2], dataBytes[3],
            dataBytes[4], dataBytes[5], dataBytes[6], dataBytes[7]);
    UART_LOG(str);
}

bool get_enable(void)
{
    return buttons.Enable;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  get_speed
 * Description   :  This function returns the value of the speed toggle
 *
 *END**************************************************************************/
bool get_speed(void)
{
    return buttons.speed;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  get_Axe_X
 * Description   :  This function returns the X axis value
 *
 *END**************************************************************************/
float get_Axe_X(void)
{
    return moves.Axe_X;
}

/*FUNCTION**********************************************************************
 *
 * Function Name :  get_Axe_Y
 * Description   :  This function returns the Y axis value
 *
 *END**************************************************************************/
float get_Axe_Y(void)
{
    return moves.Axe_Y;
}
