#include <stdio.h>

// local includes
#include "FAT.h"
#include "PORT.h"
#include "read_sector.h"


// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

uint8_t read8 (uint16_t offset, uint8_t * array_name){

}

uint16_t read16 (uint16_t offset, uint8_t * array_name){

}

uint32_t read32 (uint16_t offset, uint8_t * array_name){
    
}

uint8_t mount_drive(void) {
    uint8_t error = NO_ERROR;
    uint8_t xdata block_data[512];
    
    read_sector(0, 512, &block_data);
    
    printf("First byte: %2.2bX", block_data[0]);
    
    return error;
}