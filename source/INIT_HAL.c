#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "fsl_clock.h"
#include "fsl_port.h"
#include "UART_HAL.h"
#include "CANopen_HAL.h"
#include "Utilities.h"

/*FUNCTION**********************************************************************
 *
 * Function Name : Init
 * Description   : This function initializes all of the components needed for running the application
 *                 Updated for MCUXpresso SDK
 * Author: Simon Falardeau (Ported to MCUXpresso)
 *END**************************************************************************/
void Init(void)
{
    BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitBootPeripherals();

	UART_LogInit();

}
