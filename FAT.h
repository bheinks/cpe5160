#ifndef _FAT_H
#define _FAT_H

#include "main.h"

// Function prototypes
uint32_t read32 (uint16_t offset, uint8_t * array_name, uint8_t bytes);
uint8_t mount_drive(void);

#endif