/*
	CpE 5160 Experiment 2
	Brett Heinkel
	Michael Proemsey
	Ian Piskulic
*/

#include <stdio.h>

// local includes
#include "main.h"
#include "PORT.h"
#include "UART.h"
#include "delay.h"
//#include "LCD.h"
//#include "SPI.h"
//#include "Long_Serial_In.h"
//#include "SD.h"
//#include "print_bytes.h"
#include "I2C.h"

// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

// SD card data block
uint8_t xdata block_data[512];

void main(void) {
    uint8_t status, error, byte_array[1];
    uint16_t i;
    uint32_t block_num;
    
    AUXR = 0x0C; // make all of XRAM available
    
    if (OSC_PER_INST == 6) {
        CKCON0 = 0x01; // set x2 clock mode
    }
    else {
        CKCON0 = 0x00; // set standard clock mode
    }
	
	// Initialize UART
	UART_init(9600);
    delay(300);
    
    i = 0;
    do {
        error = I2C_read(0x43, 0x01, 1, 1, byte_array);
        i++;
    } while ((error != 0) && (i != 0));
    printf("Received Value = %2.2bX\n\r", byte_array[0]);
    printf("Return value = %2.2bX\n\r", error);
    
    /*
	// Initialize SPI at 400 KHz
    SPI_master_init(400000);
    
    // Initialize SD card
    printf("SD Card Initialization ... \n\r");
    status = SD_card_init();

    if(status ==  NO_ERROR) {
        green = 0;
    }
    else { // pause program to allow error message to be read
        while(1);
    }
   
    // Set SPI clock to 25 MHz
    SPI_master_init(25000000UL);
   
    // initialize SD data block array
    for(i = 0; i < 512; ++i) {
        block_data[i] = 0xFF;
    }
   
    // Super Loop
    while (1) {
        // Get block number from user
        printf("Enter block number = ");
        block_num = long_serial_input();
        
        // Send command and grab block
        nCS0 = 0;
        status = send_command(CMD17, block_num);
        
        read_block(512, &block_data); 
        print_memory(block_data, 512);
    }*/
    
    while (1);
}