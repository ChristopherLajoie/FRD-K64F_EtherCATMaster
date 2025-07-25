#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "INIT_HAL.h"

int main(void) {

 Init();
 UART_LogSetEnabled(true);

#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    BOARD_InitDebugConsole();
#endif

    UART_LOG("Hello World\r\n");
    volatile static int i = 0 ;

    while(1) {
        i++ ;
        UART_LogProcess();
        __asm volatile ("nop");
    }
    return 0 ;
}


