#include "Utilities.h"
#include "UART_HAL.h"
#include <string.h>
#include <stdio.h>

//==============================================================================
// UART Logging System - Simplified for Blocking UART
//==============================================================================

typedef struct {
    char buffer[UART_LOG_BUFFER_SIZE];
    volatile uint16_t writeIndex;
    volatile uint16_t readIndex;
    volatile bool overflow;
} uart_log_t;

// Static variables for UART logging system
static uart_log_t uartLog = {0};
static bool uartLogEnabled = true;  // Enable by default

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogInit
 * Description   : Initialize the UART logging system. Clears all buffers and
 *                 enables logging by default.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_LogInit(void)
{
    uartLog.writeIndex = 0;
    uartLog.readIndex = 0;
    uartLog.overflow = false;
    uartLogEnabled = true;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogSetEnabled
 * Description   : Enable or disable UART logging at runtime. When disabled,
 *                 UART_LogMessage() calls will be ignored.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_LogSetEnabled(bool enabled)
{
    uartLogEnabled = enabled;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogIsEnabled
 * Description   : Check if UART logging is currently enabled.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool UART_LogIsEnabled(void)
{
    return uartLogEnabled;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogMessage
 * Description   : Add a message to the UART log buffer. This function is
 *                 non-blocking and ISR-safe. Messages are stored in an internal
 *                 buffer and transmitted later by UART_LogProcess().
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_LogMessage(const char* message)
{
    if (!uartLogEnabled || !message) {
        return;
    }

    uint16_t msgLen = strlen(message);
    if (msgLen >= UART_MAX_LOG_ENTRY_SIZE) {
        msgLen = UART_MAX_LOG_ENTRY_SIZE - 1;
    }

    // Calculate space needed (length byte + message + null terminator)
    uint16_t spaceNeeded = msgLen + 2;
    uint16_t nextWriteIndex = (uartLog.writeIndex + spaceNeeded) % UART_LOG_BUFFER_SIZE;

    // Check if buffer would overflow
    if (((nextWriteIndex > uartLog.readIndex) &&
         (uartLog.writeIndex < uartLog.readIndex)) ||
        ((nextWriteIndex > uartLog.readIndex) &&
         (uartLog.writeIndex >= uartLog.readIndex) &&
         (nextWriteIndex - uartLog.readIndex >= UART_LOG_BUFFER_SIZE - spaceNeeded))) {

        uartLog.overflow = true;
        return; // Drop message to avoid corruption
    }

    // Store message length first
    uartLog.buffer[uartLog.writeIndex] = (char)msgLen;
    uartLog.writeIndex = (uartLog.writeIndex + 1) % UART_LOG_BUFFER_SIZE;

    // Store message
    for (uint16_t i = 0; i < msgLen; i++) {
        uartLog.buffer[uartLog.writeIndex] = message[i];
        uartLog.writeIndex = (uartLog.writeIndex + 1) % UART_LOG_BUFFER_SIZE;
    }

    // Null terminator
    uartLog.buffer[uartLog.writeIndex] = '\0';
    uartLog.writeIndex = (uartLog.writeIndex + 1) % UART_LOG_BUFFER_SIZE;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogProcess
 * Description   : Process and output all queued UART log messages using
 *                 blocking UART transmission. Much simpler than interrupt version.
 *
 * Auteur: Simon Falardeau (Simplified for blocking UART)
 *END**************************************************************************/
void UART_LogProcess(void)
{
    static bool overflowReported = false;

    // Report log buffer overflow if it occurred
    if (uartLog.overflow && !overflowReported) {
        UART_Write_NonBlocking("UART_LOG: Buffer overflow - messages lost\r\n");
        overflowReported = true;
        uartLog.overflow = false;
    }

    // Process all queued log messages - much simpler now!
    while (uartLog.readIndex != uartLog.writeIndex) {
        // Read message length
        uint8_t msgLen = (uint8_t)uartLog.buffer[uartLog.readIndex];
        uartLog.readIndex = (uartLog.readIndex + 1) % UART_LOG_BUFFER_SIZE;

        // Sanity check
        if (msgLen == 0 || msgLen >= UART_MAX_LOG_ENTRY_SIZE) {
            UART_LogInit();
            UART_Write_NonBlocking("UART_LOG: Buffer corrupted, reset\r\n");
            return;
        }

        // Read message from log buffer
        char tempMsg[UART_MAX_LOG_ENTRY_SIZE];
        for (uint8_t i = 0; i <= msgLen; i++) { // Include null terminator
            tempMsg[i] = uartLog.buffer[uartLog.readIndex];
            uartLog.readIndex = (uartLog.readIndex + 1) % UART_LOG_BUFFER_SIZE;
        }

        // Send message using blocking UART - no retry logic needed!
        UART_Write_NonBlocking(tempMsg);

        overflowReported = false;
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_LogHasMessages
 * Description   : Check if there are messages waiting to be processed in the
 *                 UART log buffer.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool UART_LogHasMessages(void)
{
    return (uartLog.readIndex != uartLog.writeIndex);
}

// Keep only the bit manipulation functions you actually use
// Remove the ones you don't need

/*FUNCTION**********************************************************************
 *
 * Function Name : delay
 * Description   : This function does nothing for a specified number of cycles.
 *                 Used for simple software delays.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void delay(volatile int cycles)
{
    /* Delay function - do nothing for a number of cycles */
    while(cycles--);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : check_bit0
 * Description   : This function checks if bit 0 of an 8-bit value is set to 1.
 *                 Returns true if set, false otherwise.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool check_bit0(uint8_t value)
{
    return ((value & 0x01) == 1);
}

bool check_bit1(uint8_t value)
{
    return ((value & 0x02) == 2);
}

bool check_bit2(uint8_t value)
{
    return ((value & 0x04) == 4);
}

bool check_bit3(uint8_t value)
{
    return ((value & 0x08) == 8);
}

bool check_bit4(uint8_t value)
{
    return ((value & 0x10) == 16);
}

bool check_bit5(uint8_t value)
{
    return ((value & 0x20) == 32);
}

bool check_bit6(uint8_t value)
{
    return ((value & 0x40) == 64);
}

bool check_bit7(uint8_t value)
{
    return ((value & 0x80) == 128);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : set_bit0
 * Description   : This function takes an 8-bit value and sets bit 0 to 1.
 *                 Returns the modified value.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
uint8_t set_bit0(uint8_t value)
{
    return value | 0x01;
}

uint8_t set_bit1(uint8_t value)
{
    return value | 0x02;
}

uint8_t set_bit2(uint8_t value)
{
    return value | 0x04;
}

uint8_t set_bit3(uint8_t value)
{
    return value | 0x08;
}

uint8_t set_bit4(uint8_t value)
{
    return value | 0x10;
}

uint8_t set_bit5(uint8_t value)
{
    return value | 0x20;
}

uint8_t set_bit6(uint8_t value)
{
    return value | 0x40;
}

uint8_t set_bit7(uint8_t value)
{
    return value | 0x80;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : check_bit0_extended
 * Description   : This function checks if bit 0 of a 32-bit value is set to 1.
 *                 Returns true if set, false otherwise.
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool check_bit0_extended(uint32_t value)
{
    return ((value & 0x00000001) == 1);
}

bool check_bit1_extended(uint32_t value)
{
    return ((value & 0x00000002) == 2);
}

bool check_bit3_extended(uint32_t value)
{
    if ((value & 0x00000008) == 8)
    {
        return true;
    }
    return false;
}

bool check_bit4_extended(uint32_t value)
{
    if ((value & 0x00000010) == 16)
    {
        return true;
    }
    return false;
}

bool check_bit5_extended(uint32_t value)
{
    if ((value & 0x00000020) == 32)
    {
        return true;
    }
    return false;
}

bool check_bit6_extended(uint32_t value)
{
    if ((value & 0x00000040) == 64)
    {
        return true;
    }
    return false;
}

bool check_bit7_extended(uint32_t value)
{
    if ((value & 0x00000080) == 128)
    {
        return true;
    }
    return false;
}

bool check_bit8_extended(uint32_t value)
{
    if ((value & 0x00000100) == 256)
    {
        return true;
    }
    return false;
}

bool check_bit9_extended(uint32_t value)
{
    if ((value & 0x00000200) == 512)
    {
        return true;
    }
    return false;
}

bool check_bit10_extended(uint32_t value)
{
    if ((value & 0x00000400) == 1024)
    {
        return true;
    }
    return false;
}

bool check_bit11_extended(uint32_t value)
{
    if ((value & 0x00000800) == 2048)
    {
        return true;
    }
    return false;
}

bool check_bit12_extended(uint32_t value)
{
    if ((value & 0x00001000) == 4096)
    {
        return true;
    }
    return false;
}

bool check_bit13_extended(uint32_t value)
{
    if ((value & 0x00002000) == 8192)
    {
        return true;
    }
    return false;
}

bool check_bit14_extended(uint32_t value)
{
    if ((value & 0x00004000) == 16384)
    {
        return true;
    }
    return false;
}

bool check_bit15_extended(uint32_t value)
{
    if ((value & 0x00008000) == 32768)
    {
        return true;
    }
    return false;
}

bool check_bit16_extended(uint32_t value)
{
    if ((value & 0x00010000) == 65536)
    {
        return true;
    }
    return false;
}

bool check_bit17_extended(uint32_t value)
{
    return ((value & 0x00020000) == 131072);
}
