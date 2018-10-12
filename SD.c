#include "SD.h"
#include "SPI.h"

sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

uint8_t send_command(uint8_t command, uint32_t argument)
{
    uint8_t rec_value, argument_LSB, argument_byte1, argument_byte2, argument_MSB, command_end, return_value, error_flag;
    
    // check if CMD value is valid (less than 64)
    if(command < 64){
        return_value = NO_ERROR;
    }
    else{
        return_value = ILLEGAL_COMMAND;
        return return_value;
    }
    
    // append start and transmission bits to the 6 bit command
    command |= 0x40;
    
    // set commandend based on necessary CRC7 and end bit
    if(command == 0x40){      // CMD0
        command_end = 0x95;
    }
    else if(command == 0x48){ // CMD8
        command_end = 0x87;
    }
    else{                    // all other commands require no checksum
        command_end = 0x01;
    }
    
    // split argument into 4 bytes
    argument_LSB = argument & 0xFF;
    argument_byte1 = (argument >> 8) & 0xFF;
    argument_byte2 = (argument >> 16) & 0xFF;
    argument_MSB = (argument >> 24) & 0xFF;
    
    // send 6 byte command while checking for errors
    error_flag = SPI_transfer(command, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }
    
    error_flag = SPI_transfer(argument_MSB, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_byte2, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_byte1, &rec_value);
    if(error_flag != NO_ERROR) {
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_LSB, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(command_end, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }
    
    return return_value;
}

uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array)
{
    uint8_t SPI_val, count, error_flag, response;
    response = NO_ERROR;
    count = 0;

    // Keep transmitting until a response is received or timeout occurs
	do
    {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    }while(((SPI_val&0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
	 
	// Error handling
    if (error_flag != NO_ERROR)
    {
        response = SPI_ERROR;
    }
	else if (count == 0)
    {
		response = TIMEOUT_ERROR;
    }
	else if ((SPI_val&0xFE) != 0x00)
    {
        *byte_array = SPI_val;
        response = COMM_ERROR;
    }
       
	// No errors found
	else
	{
        *byte_array = SPI_val;
        if(num_bytes > 1) // If the response is greater than one byte
        {
            for(count = 1; count < num_bytes; count++)
            {
                error_flag = SPI_transfer(0xFF, &SPI_val);
                *(byte_array + count) = SPI_val;
            }
        }
        else
        {
          response = COMM_ERROR;
        }
    }
    error_flag = SPI_transfer(0xFF, &SPI_val);  // End with sending one last 0xFF out of the SPI port
			
    return response;
}

uint8_t SD_card_init(void) {
    uint8_t index, return_value, receive_array[8], error_flag, error_status, timeout;

    timeout = 0; //initialize timeout variable
    
    printf("Initializing SD card...\n");
    
    //74+ clock pulses on SCK with nCS high
    nCS0 = 1;
    for(index = 0; index < 10; ++index) {
        error_flag = SPI_transfer(0xFF, &return_value);
    }
    
    /************
    *
    *  command 0
    *
    *************/
    
    green = 0;
    printf("CMD0 sent...\n");
    
    nCS0 = 0;
    
    //check for error
    if(error_flag != NO_ERROR){
        return_value = SD_INIT_ERROR;
        return return_value;
    }
    
    //send CMD0 to SD card
    error_flag = send_command(CMD0, 0);
    
    //check for error
    if(error_flag != NO_ERROR){
        return_value = SD_INIT_ERROR;
        return return_value;
    }
    
    //recieve response from SD card
    error_flag=recieve_response(1, recieve_array);
    nCS0 = 1;
    
    if(error_flag != NO_ERROR){
        error_status = error_flag;
        return_value = SD_INIT_ERROR;
        return return_value;
    }
    else if(rec_array[0] != 0x01){
        error_status = response_error;
        return_value = SD_INIT_ERROR;
        printf(error_status);
        printf("Response recieved: 0x");
        printf("%2.2Bx", recieve_array[0]);
        printf("...\n");
        return return_value;
    }
    
    printf("Response recieved: 0x");
    printf("%2.2Bx", recieve_array[0]);
    printf("...\n");
    
    /************
    *
    *  command 8
    *
    *************/
    
    nCS0 = 1;
    //send CMD8 to SD card
    error_flag = send_command(CMD8, 0x000001AA);
    
    //check for error
    if(error_flag != NO_ERROR){
        return_value = SD_INIT_ERROR;
        return return_value;
    }
    
    //recieve response from SD card
    error_flag=recieve_response(5, recieve_array);
    nCS0 = 1;
    
    if(recieve_array[0] == 0x05){
        error_status = VERSION_1;
        printf("Version 1 SD Card is Not Supported \n);
        return return_value;
    }
    else if(rec_array[0] != 0x01){
        error_status = response_error;
        return_value = SD_INIT_ERROR;
        printf(error_status);
        printf("Response recieved: 0x");
        printf("%2.2Bx", recieve_array[0]);
        printf("...\n");
        return return_value;
    }
    
    printf("Response recieved: 0x");
    printf("%2.2Bx", recieve_array[0]);
    printf("...\n");
}