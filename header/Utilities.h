#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "fsl_common.h"

//==============================================================================
// UART Logging System Configuration
//==============================================================================
#define UART_LOG_BUFFER_SIZE 4096
#define UART_MAX_LOG_ENTRY_SIZE 64

//==============================================================================
// UART Logging Macros
//==============================================================================
#define UART_LOG(msg) UART_LogMessage(msg)

#define UART_PRINTF(fmt, ...) do { \
    char uart_log_buf[128]; \
    sprintf(uart_log_buf, fmt, __VA_ARGS__); \
    UART_LogMessage(uart_log_buf); \
} while(0)

//==============================================================================
// UART Logging System Functions
//==============================================================================

// Initialization and control functions
void UART_LogInit(void);
void UART_LogSetEnabled(bool enabled);
bool UART_LogIsEnabled(void);

// Core logging functions
void UART_LogMessage(const char* message);
void UART_LogProcess(void);

// Status and utility functions
bool UART_LogHasMessages(void);

//==============================================================================
// Utility Functions
//==============================================================================

// Delay function
void delay(volatile int cycles);

// Check bit functions for 8-bit integers
bool check_bit0(uint8_t value);
bool check_bit1(uint8_t value);
bool check_bit2(uint8_t value);
bool check_bit3(uint8_t value);
bool check_bit4(uint8_t value);
bool check_bit5(uint8_t value);
bool check_bit6(uint8_t value);
bool check_bit7(uint8_t value);

// Set bit functions for 8-bit integers
uint8_t set_bit0(uint8_t value);
uint8_t set_bit1(uint8_t value);
uint8_t set_bit2(uint8_t value);
uint8_t set_bit3(uint8_t value);
uint8_t set_bit4(uint8_t value);
uint8_t set_bit5(uint8_t value);
uint8_t set_bit6(uint8_t value);
uint8_t set_bit7(uint8_t value);

// Check bit functions for 32-bit integers
bool check_bit0_extended(uint32_t value);
bool check_bit1_extended(uint32_t value);
bool check_bit3_extended(uint32_t value);
bool check_bit4_extended(uint32_t value);
bool check_bit5_extended(uint32_t value);
bool check_bit6_extended(uint32_t value);
bool check_bit7_extended(uint32_t value);
bool check_bit8_extended(uint32_t value);
bool check_bit9_extended(uint32_t value);
bool check_bit10_extended(uint32_t value);
bool check_bit11_extended(uint32_t value);
bool check_bit12_extended(uint32_t value);
bool check_bit13_extended(uint32_t value);
bool check_bit14_extended(uint32_t value);
bool check_bit15_extended(uint32_t value);
bool check_bit16_extended(uint32_t value);
bool check_bit17_extended(uint32_t value);

#endif /*UTILITIES_H*/
