#ifndef _UART_H
#define _UART_H

#include "main.h"

// UART settings
#define UART_MODE_0 0x00
#define UART_MODE_1 0x40
#define UART_MODE_2 0x80
#define UART_MODE_3 0xC0
#define ENABLE_485_MODE 0x20
#define RECEIVE_ENABLE 0x10

#define SMOD1 1
#define SPD 1

#define UART_TIMEOUT 0xF1

// ASCII characters
#define SPACE 0x20
#define CR 0x0D
#define LF 0x0A
#define BS 0x08
#define DEL 0x7F

void UART_init(uint16_t BAUD_RATE);
uint8_t UART_transmit(uint8_t send_value);
uint8_t UART_receive(void);

#endif

/*
#define SMOD1 (0)
#define SPD (1)
#define BAUD_RATE (9600)
#define BRL_CALC ((uint8_t)(256-(((1+(5*SPD))*(1+(1*SMOD1))*OSC_FREQ)/(32*OSC_PER_INST*BAUD_RATE))))

void UART_Init(void);
uint8_t UART_Transmit(uint8_t send_value);
uint8_t UART_Receive(void);

#endif
*/
