#ifndef _uart_H
#define _uart_H

#include "Main.h"

#define SMOD1 (0)
#define SPD (1)
#define BAUD_RATE (9600)
#define BRL_CALC ((uint8_t)(256-(((1+(5*SPD))*(1+(1*SMOD1))*OSC_FREQ)/(32*OSC_PER_INST*BAUD_RATE))))

void UART_Init(void);
uint8_t UART_Transmit(uint8_t send_value);
uint8_t UART_Receive(void);

#endif