#include "UART_HAL.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_INSTANCE          UART0
#define UART_CLK_FREQ          CLOCK_GetFreq(UART0_CLK_SRC)

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t buffer[BUFFER_SIZE];
uint8_t bufferIdx = 0;

/*******************************************************************************
 * Reception Functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_DataAvailable
 * Description   : Check if data is available to read (non-blocking check)
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool UART_DataAvailable(void)
{
    return (UART_GetStatusFlags(UART_INSTANCE) & kUART_RxDataRegFullFlag);
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_ReadLine
 * Description   : Read a complete line from UART (up to newline or buffer full)
 *                 This is blocking - only call when data is available
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
size_t UART_ReadLine(void)
{
    bufferIdx = 0;
    uint8_t ch;

    // Read characters until newline or buffer full
    while (bufferIdx < (BUFFER_SIZE - 1)) {
        // Blocking read of one character
        if (UART_ReadBlocking(UART_INSTANCE, &ch, 1) == kStatus_Success) {
            buffer[bufferIdx++] = ch;

            // Stop at newline
            if (ch == '\n') {
                break;
            }
        }
    }

    // Null terminate
    buffer[bufferIdx] = '\0';
    return bufferIdx;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_ClearBuffer
 * Description   : Clear the RX buffer
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_ClearBuffer(void)
{
    buffer[0] = 0;
    bufferIdx = 0;
}

/*******************************************************************************
 * Transmission Functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Write_Blocking
 * Description   : Send message using blocking method
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_Write_Blocking(const char *message)
{
    if (!message) {
        return;
    }

    // Simple blocking write - SDK handles everything
    UART_WriteBlocking(UART_INSTANCE, (uint8_t*)message, strlen(message));
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Write_NonBlocking
 * Description   : Send message - actually blocking for simplicity
 *                 Kept for compatibility with logging system
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
status_t UART_Write_NonBlocking(const char* message)
{
    if (!message) {
        return kStatus_Fail;
    }

    // For simplicity, just use blocking write
    // In EtherCAT context, this will only be called during idle periods
    UART_Write_Blocking(message);
    return kStatus_Success;
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_Write
 * Description   : Default write function (blocking)
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_Write(const char *message)
{
    UART_Write_Blocking(message);
}

/*******************************************************************************
 * Application Functions
 ******************************************************************************/

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_echo
 * Description   : Echo received data with command processing
 *                 Keeps your original S32Studio behavior
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
void UART_echo(void)
{
    if (bufferIdx == 0) {
        return;
    }

    /* If the received string is "Hello Board", send back "pipeline" */
    if(strcmp((char *)buffer, "Hello Board\n") == 0)
    {
        UART_Write_Blocking("pipeline \n");
    }
    else
    {
        /* Echo back the received data */
        UART_Write_Blocking((char*)buffer);
    }
}

/*FUNCTION**********************************************************************
 *
 * Function Name : UART_HAL_send_data
 * Description   : Simple test function for development
 *
 * Auteur: Simon Falardeau
 *END**************************************************************************/
bool UART_HAL_send_data(char data)
{
    return (data == 'b');
}
