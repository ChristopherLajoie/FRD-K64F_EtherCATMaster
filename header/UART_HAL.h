#ifndef UART_HAL_H
#define UART_HAL_H

#include "fsl_uart.h"
#include "fsl_clock.h"
#include "board.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 256U

/*! Welcome message displayed at the console */
#define welcomeMsg "\r\n\r\nThis example is a simple echo using UART\r\n\
it will send back any character you send to it.\r\n\
The board will greet you if you send 'Hello Board'\r\
\nNow you can begin typing:\r\n"

extern uint8_t buffer[BUFFER_SIZE];     /*!< RX buffer for console input */
extern uint8_t bufferIdx;               /*!< Current index in RX buffer */

/*******************************************************************************
 * API Functions
 ******************************************************************************/

/*!
 * @brief Check if data is available to read
 * @return true if data available, false otherwise
 */
bool UART_DataAvailable(void);

/*!
 * @brief Read a line from UART (blocking until newline or buffer full)
 * @return Number of bytes read
 */
size_t UART_ReadLine(void);

/*!
 * @brief Clear the RX buffer
 */
void UART_ClearBuffer(void);

/*!
 * @brief Send message using blocking method
 * @param message Null-terminated string to send
 */
void UART_Write_Blocking(const char *message);

/*!
 * @brief Send message using non-blocking method (for logging compatibility)
 * @param message Null-terminated string to send
 * @return kStatus_Success if sent, kStatus_Fail if failed
 */
status_t UART_Write_NonBlocking(const char* message);

/*!
 * @brief Default write function (blocking)
 * @param message Null-terminated string to send
 */
void UART_Write(const char *message);

/*!
 * @brief Echo received data back to sender with command processing
 */
void UART_echo(void);

/*!
 * @brief Simple test function for development
 * @param data Character to test
 * @return true if data is 'b', false otherwise
 */
bool UART_HAL_send_data(char data);

#endif /*UART_HAL_H*/
