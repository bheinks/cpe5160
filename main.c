/*
	CpE 5160 Experiment 2
	Brett Heinkel
	Michael Proemsey
	Ian Piskulic
*/

#include <stdio.h>

#include "main.h"
#include "PORT.h"
#include "UART.h"
#include "Long_Serial_In.h"
#include "delay.h"
#include "LCD.h"

sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

uint8_t code line1_string[] = "Line 1 test";
uint8_t code line2_string[] = "Line 2 test";

void main(void) {
    AUXR = 0x0C; // make all of XRAM available
    
    if (OSC_PER_INST == 6) {
        CKCON0 = 0x01; // set x2 clock mode
    }
    else {
        CKCON0 = 0x00; // set standard clock mode
    }
    
    green = 0;
	
	UART_init(9600);
    
	yellow = 0;
    delay(300);
	
	
    LCD_init();
    LCD_print(LINE1, 0, line1_string);
    LCD_print(LINE2, 0, line2_string);
    
    red = 0;

	while(1) {
        UART_transmit('f');
        UART_transmit('a');
        UART_transmit('r');
        UART_transmit('t');
        UART_transmit(CR);
        UART_transmit(LF);
        delay(1000);
    }
}
