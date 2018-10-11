#include "SD.h"
#include "main.h"
#include "SPI.h"

uint8_t send_command(uint8_t command, uint32_t argument)
{
    command &= 0x40; // append start and transmission bits to the 6 bit command
    
    
    