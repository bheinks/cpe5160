#ifndef _FAT_H
#define _FAT_H

#include "main.h"

// Function prototypes
uint8_t read8 (uint16_t offset, uint8_t * array_name); 
uint16_t read16 (uint16_t offset, uint8_t * array_name); 
uint32_t read32 (uint16_t offset, uint8_t * array_name);
uint8_t mount_drive(void);

#endif