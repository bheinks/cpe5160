#ifndef _SPI_H
#define _SPI_H

#include "main.h"

#define CPOL 0
#define CPHA 0

// errors
#define CLOCK_RATE_ERROR 0xF1
#define TIMEOUT_ERROR 0xF2
#define SPI_ERROR 0xF3
#define NO_ERROR 0x00

uint8_t SPI_master_init(uint32_t clock_freq);
uint8_t SPI_transfer(uint8_t send_value, uint8_t * rec_value);

#endif