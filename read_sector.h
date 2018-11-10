#ifndef _READ_SECTOR_H
#define _READ_SECTOR_H

#include "main.h"

#define DISK_ERROR 0xE0

uint8_t read_sector(uint32_t sector_number, uint16_t sector_size, uint8_t * byte_array);

#endif