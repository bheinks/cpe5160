/*
	CpE 5160 Experiment 1
	Brett Heinkel
	Michael Proemsey
	Ian Piskulic
*/

#include <stdio.h>

#include "Main.h"
#include "PORT.h"
#include "uart.h"
#include "memory_test.h"
#include "print_bytes.h"
#include "LCD.h"

sbit green = P2^7;
sbit orange = P2^6;

uint8_t code LCD_string1[] = "Brett Heinkel";

void main(void) {	
	CKCON0 = 0x01;
	
	UART_Init();
	
	green = 0;
	hardware_delay(2000);
	orange = 0;
	
	LCD_Init();
	LCD_Write(1, set_address | line2);
	LCD_Print(12, LCD_string1);
	
	/*hardware_delay(1);
	LCD_Write(1, set_address | line2);
	LCD_Output_Display(11, LCD_string2);*/
	
	/*code_memory = code_memory_init();
	xdata_memory = xdata_memory_init();
	
	printf("Printing from code memory:\n");
	print_memory(code_memory, 50);
	
	//DELAY_LOOP_Wait(2000);
	
	printf("Printing from xdata memory:\n");
	print_memory(xdata_memory, 50);*/

	while(1);
}

void hardware_delay(const unsigned int DELAY) {
	unsigned int x, y;

	for (x = 0; x <= DELAY; x++) {
		for (y = 0; y <= 575; y++);
	}
}