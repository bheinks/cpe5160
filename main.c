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
#include "STA013.h"

// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;
sbit btn = P2^3;

// SD card data block
//uint8_t xdata block_data[512];

void main(void) {
    uint8_t status, error, byte_array[1], i;
    //uint32_t block_num;
    
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
    
    //STA013_init();
    
    while(btn != 0);
    green = 0;
    
    i = 0;
    do {
        error = I2C_read(0x43, 0x01, 1, 1, byte_array);
        i++;
    } while ((error != 0) && (i != 0));
    printf("Received value: %2.2bX\n", byte_array[0]);
    
    green = 1;
    while (1);
}