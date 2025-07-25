#ifndef CANOPEN_HAL_H
#define CANOPEN_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include "fsl_flexcan.h"
#include "fsl_common.h"
#include "fsl_clock.h"

//CANopen Address IDs
#define MOVES_ID    0x28A
#define BUTTONS_ID  0x18A
#define LED_ID      0x30A

//TX and RX mailbox definitions for MOVES
#define TX_MOVES_MAILBOX    (1UL)
#define TX_MOVES_MSG_ID     (1UL)
#define RX_MOVES_MAILBOX    (0UL)
#define RX_MOVES_MSG_ID     (MOVES_ID)

//TX and RX mailbox definitions for BUTTONS
#define TX_BUTTONS_MAILBOX  (2UL)
#define TX_BUTTONS_MSG_ID   (2UL)
#define RX_BUTTONS_MAILBOX  (3UL)
#define RX_BUTTONS_MSG_ID   (BUTTONS_ID)

//TX and RX mailbox definitions for LED
#define TX_LED_MAILBOX      (4UL)
#define TX_LED_MSG_ID       (4UL)
#define RX_LED_MAILBOX      (5UL)
#define RX_LED_MSG_ID       (LED_ID)

// FlexCAN instance to use (adjust based on your hardware)
#define FLEXCAN_INSTANCE    CAN0
#define CAN0_PERIPHERAL     CAN0

// Data structures for CAN messages
typedef struct {
    float Axe_X;
    float Axe_Y;
} Moves;

typedef struct {
    bool Yellow_Bat_Led;
    bool Red_Bat_Led;
    bool Overload_Led;
    bool Aux_Led;
} Led;

typedef struct {
    bool Enable;
    bool speed;
    bool E_Stop;
    bool Horn;
    bool CAN_Enable;
} Buttons;

// External variables
extern Moves moves;
extern Led led;
extern Buttons buttons;

// Data transmission
void SendCANData(uint32_t mailbox, uint32_t messageId, uint8_t * data, uint32_t len);

// Data reception
status_t FLEXCAN_Receive_Moves(void);
status_t FLEXCAN_Receive_Buttons(void);
status_t FLEXCAN_Receive_Led(void);

// Getter functions
float get_Axe_X(void);
float get_Axe_Y(void);
bool get_speed(void);
bool get_enable(void);

// Debugging functions
void Show_movesRecvBuffer(void);
void Show_buttonsRecvBuffer(void);
void Show_ledRecvBuffer(void);

#endif /* CANOPEN_HAL_H */
