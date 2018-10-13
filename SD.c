#include "SD.h"
#include "SPI.h"
#include <STDIO.H>
#include "PORT.h"

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

uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array) {
    uint8_t SPI_val, count, error_flag, response;
    
    response = NO_ERROR;
    count = 0;

    // Keep transmitting until a response is valid received or timeout occurs
	do
    {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val&0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
    
	// Error handling
    if (error_flag != NO_ERROR)
    {
        response = SPI_ERROR;
        printf("SPI_ERROR\n");
    }
	else if (count == 0)
    {
		response = TIMEOUT_ERROR;
        printf("TIMEOUT_ERROR\n");
    }
    else {
        *byte_array = SPI_val;
     
        // if valid R1 response (active or idle)
        if ((SPI_val == 0x00) || (SPI_val == 0x01)) {
            if (num_bytes > 1) {
                for (count = 1; count < num_bytes; ++count) {
                    error_flag = SPI_transfer(0xFF, &SPI_val);
                    *(byte_array + count) = SPI_val;
                }
            }
        }
        else {
            response = COMM_ERROR;
            printf("COMM_ERROR\n");
            printf("0x%2.2Bx\n", SPI_val);
            return response;
        }
    }
    
    // End with sending one last 0xFF out of the SPI port
    error_flag = SPI_transfer(0xFF, &SPI_val);
    return response;
}

uint8_t SD_card_init(void) {
    uint8_t index, receive_array[8], error_flag, timeout, return_value, i, error_message;

    timeout = 1; //initialize timeout variable
    
    printf("Initializing SD card...\n");
    
    // 74+ clock pulses on SCK with nCS high
    nCS0 = 1;
    for(index = 0; index < 10; ++index) {
        error_flag = SPI_transfer(0xFF, &return_value);
    }
    
    /************
    *
    *  command 0
    *
    *************/
    

    
    
    
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("SCK init error\n");
        return SD_INIT_ERROR;
    }
    
    //send CMD0 to SD card
    printf("CMD0 sent...\n");
    nCS0 = 0;
    error_flag = send_command(CMD0, 0);
    
    //check for error
    if(error_flag != NO_ERROR){
        error_message = SEND_ERROR;
    }
    
    //receive response from SD card
    error_flag=receive_response(1, receive_array);
    nCS0 = 1;
    printf("CMD0 R1 Response expected\n");
    if(error_flag != NO_ERROR){
        if(error_message == SEND_ERROR){
            printf("CMD0 send error\n");
        }
        else{
            printf("CMD0 receive error\n");
        }
        return SD_INIT_ERROR;
    }
    else if(receive_array[0] != 0x01){
        printf("CMD0 response incorrect");
        printf("Response received: 0x");
        printf("%2.2Bx", receive_array[0]);
        printf("...\n");
        return SD_INIT_ERROR;
    }
    
    printf("Response received: 0x");
    printf("%2.2Bx", receive_array[0]);
    printf("...\n");
    green = 0;
    /************
    *
    *  command 8
    *
    *************/
    
    nCS0 = 0;
    // Send CMD8 to SD card
    printf("CMD8 sent...\n");
    error_flag = send_command(CMD8, 0x000001AA);
    
    // Check for error
    if(error_flag != NO_ERROR){
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(6, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        if(error_message == SEND_ERROR){
            printf("CMD8 send error\n");
        }
        else{
        printf("CMD8 receive error\n");
        }
        return SD_INIT_ERROR;
    }
    // Print results
    printf("CMD8 R7 Response expected\n");
    if(receive_array[0] == 0x05){
        printf("Version 1 SD Card is Not Supported\n");
        return SD_INIT_ERROR;
    }
    else{
        printf("Version 2 Card Detected\n");
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2Bx", receive_array[i]);
        }
        printf("...\n");
    }
    
    // Check for correct voltage
    if((receive_array[3]&0x0F) != 0x01){
        printf("CMD8 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    
    // Check for matching check byte
    if(receive_array[4] != 0xAA){
        printf("CMD8 check byte mismatch\n");
        return SD_INIT_ERROR;
    }
    
    /************
    *
    *  CMD58
    *
    *************/
        
    nCS0 = 0;
    // Send CMD58 to SD card
    printf("CMD58 sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if(error_flag != NO_ERROR){
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        if(error_message == SEND_ERROR){
            printf("CMD58 send error\n");
        }
        else{
        printf("CMD58 receive error\n");
        }
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("CMD58 R3 Response expected\n");
    if((receive_array[2]&0xFC) != 0xFC){
        printf("CMD58 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    else{
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2Bx", receive_array[i]);
        }
        printf("...\n");
    }
    
    /************
    *
    *  ACMD41
    *
    *************/
    // Sending command CMD55 and ACMD41 until the R1 response is 0 or a timeout occurs
    printf("ACMD41 sending...\n");
    while(receive_array[0] != 0){
        
        // Send CMD55 to SD card
        nCS0 = 0;
        error_flag = send_command(CMD55, 0);
    
        // Check for error
        if(error_flag != NO_ERROR){
            error_message = SEND_ERROR;
        }
    
        // Receive response from SD card
        error_flag=receive_response(1, receive_array);
    
        // Check for error
        if(error_flag != NO_ERROR){
            if(error_message == SEND_ERROR){
                printf("CMD55 send error\n");
        }
        else{
        printf("CMD55 receive error\n");
        }
        return SD_INIT_ERROR;
    }
        
        // Send ACMD41 to SD card
        error_flag = send_command(ACMD41, 0x40000000);
    
        // Check for error
        if(error_flag != NO_ERROR){
            printf("ACMD41 send error\n");
            return SD_INIT_ERROR;
        }
    
        // Receive response from SD card
        error_flag=receive_response(1, receive_array);
    
        // Check for error
        if(error_flag != NO_ERROR){
            printf("ACMD41 receive error\n");
            return SD_INIT_ERROR;
        }
        
        // Incriment timeout and check to see if it has reloaded
        timeout++;
        if(timeout == 0){
            printf("ACMD41 timeout error\n");
            return SD_INIT_ERROR;
        }
        nCS0 = 1;
    }
    
    // Print results
    printf("ACMD41 R1 Response Expected\n");
    printf("Response received: 0x");
    printf("%2.2Bx", receive_array[0]);
    printf("...\n");

    /************
    *
    *  CMD58 Again
    *
    *************/
        
    nCS0 = 0;
    // Send CMD58 to SD card
    printf("CMD58 sent...\n");
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if(error_flag != NO_ERROR){
        error_message = SEND_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        if(error_message == SEND_ERROR){
            printf("CMD58-2 send error\n");
        }
        else{
        printf("CMD58-2 receive error\n");
        }
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("R3 Response expected\n");
    if((receive_array[1]&0x80) != 0x80){
        printf("CMD58 card not in active state error\n");
        return SD_INIT_ERROR;
    }
    else if((receive_array[1]&0xC0) == 0xC0){
        printf("High capacity card accepted\n");
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2Bx", receive_array[i]);
        }
        printf("...\n");
    }
    else{
        printf("High capacity card not detected error");
        return SD_INIT_ERROR;
    }
        
    printf("Initialization of SD card complete...\n");
    
    return NO_ERROR;
}

uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array) {
    uint8_t SPI_val, count, error_flag, response;
    
    response = NO_ERROR;
    count = 0;
    
    // keep transmitting until a response is received or timeout occurs
	do {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val & 0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
    
    // error handling
    if (error_flag != NO_ERROR) {
        response = SPI_ERROR;
    }
	else if (count == 0) {
		response = TIMEOUT_ERROR;
    }
    else {
        if (SPI_val == 0x00) {
            count = 0;
            //orange = 0;
            
            // wait for data token
            do {
                error_flag = SPI_transfer(0xFF, &SPI_val);
                count++;
            } while ((SPI_val == 0xFF) && (error_flag == NO_ERROR) && (count != 0));
            
            if (error_flag != NO_ERROR) {
                orange = 0;
                response = SPI_ERROR;
            }
            else if (count == 0) {
                yellow = 0;
                response = TIMEOUT_ERROR;
            }
            else if (SPI_val == 0xFE) {
                red = 0;
                for (count = 0; count < num_bytes; ++count) {
                    error_flag = SPI_transfer(0xFF, &SPI_val);
                    *(byte_array + count) = SPI_val;
                }
                
                // discard CRC
                error_flag = SPI_transfer(0xFF, &SPI_val);
                error_flag = SPI_transfer(0xFF, &SPI_val);
            }
            else {
                green = 0;
                response = DATA_ERROR;
            }
        }
        else {
            response = COMM_ERROR;
        }
    }
    
    // return to standby state
    error_flag = SPI_transfer(0xFF, &SPI_val);
    
    return response;
}