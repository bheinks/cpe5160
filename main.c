/*
	CpE 5160 Experiment 3
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
#include "I2C.h"
#include "STA013.h"

// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;
sbit btn = P2^3;

void main(void) {
    uint8_t error, byte_array[1], i;

    // make all of XRAM available
    AUXR = 0x0C;
    
    if (OSC_PER_INST == 6) {
        CKCON0 = 0x01; // set x2 clock mode
    }
    else {
        CKCON0 = 0x00; // set standard clock mode
    }
	
	// Initialize UART
	UART_init(9600);
    delay(300);
    
    // initialize STA013
    STA013_init();
    
    // wait for button
    while(btn != 0);
    
    i = 0;
    do {
        error = I2C_read(0x43, 0x54, 1, 1, byte_array);
        i++;
    } while ((error != 0) && (i != 0));
    printf("Received value: %2.2bX\n", byte_array[0]);
    
    while (1);
}