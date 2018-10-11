#include "SD.h"

#include "SPI.h"

uint8_t send_command(uint8_t command, uint32_t argument)
{
    uint8_t rec_value, argument_LSB, argument_byte1, argument_byte2, argument_MSB, command_end, return_value, error_flag;
    
    //check if CMD value is valid (less than 64)
    if(command < 64)
        return_value = NO_ERROR;
    else
    {
        return_value = ILLEGAL_COMMAND;
        return return_value;
    }
    
    // append start and transmission bits to the 6 bit command
    command &= 0x40;
    
    //set commandend based on necessary CRC7 and end bit
    if(command == 0x40)      //CMD0
        command_end = 0x95;
    else if(command == 0x48) //CMD8
        command_end = 0xAA;
    else                     //all other commands require no checksum
        command_end = 0x55; 
    
    //split argument into 4 bytes
    argument_LSB = argument & 0xFF;
    argument_byte1 = (argument >> 8) & 0xFF;
    argument_byte2 = (argument >> 16) & 0xFF;
    argument_MSB = (argument >> 24) & 0xFF;
    

    
    //send 6 byte command while checking for errors
    error_flag = SPI_transfer(command, &rec_value);
    if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    error_flag = SPI_transfer(argument_MSB, &rec_value);
        if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    error_flag = SPI_transfer(argument_byte2, &rec_value);
        if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    error_flag = SPI_transfer(argument_byte1, &rec_value);
        if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    error_flag = SPI_transfer(argument_LSB, &rec_value);
        if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    error_flag = SPI_transfer(command_end, &rec_value);
        if(error_flag != NO_ERROR)
    {
        return_value = SPI_ERROR;
        return return_value;
    }    
    return return_value;
}