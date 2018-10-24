#ifndef _I2C_H
#define _I2C_H

#include "main.h"

#define NO_ERROR 0
#define BUS_BUSY_ERROR 0x81
#define NACK_ERROR 0x82

uint8_t I2C_write(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array);
uint8_t I2C_read(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array);

#endif