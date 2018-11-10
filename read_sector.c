#include "read_sector.h"
#include "SD.h"
#include "PORT.h"

uint8_t read_sector(uint32_t sector_number, uint16_t sector_size, uint8_t * byte_array) {
    uint8_t error = NO_ERROR;
    
    nCS0 = 0;
    
    error = send_command(CMD17, sector_number);
    if (error == NO_ERROR) {
        error = read_block(sector_size, byte_array);
    }
    
    nCS0 = 1;
    
    if (error != NO_ERROR) {
        error = DISK_ERROR;
    }
    
    return error;
}