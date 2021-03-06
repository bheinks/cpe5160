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

// ASCII characters
#define SPACE (0x20)
#define CR (0x0D)
#define LF (0x0A)
#define BS (0x08)
#define DEL (0x7F)

#define UART_TIMEOUT 0xF1

// function prototypes
void UART_init(uint16_t baud_rate);
uint8_t UART_transmit(uint8_t send_value);
uint8_t UART_receive(void);
uint32_t long_serial_input(void);

#endif