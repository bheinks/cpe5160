#include "LCD.h"

void LCD_Write(bit RegSelect, uint8_t message) {
  RS=RegSelect;
  E=1;
  P0=message;   
  E=0;
  P0=0xFF;
}

void LCD_Init(void) {
	hardware_delay(30);
	LCD_Write(command, function_set); // Function set x4
	hardware_delay(5);										
	LCD_Write(command, function_set);
	hardware_delay(1);								
	LCD_Write(command, function_set);
	hardware_delay(1);
	LCD_Write(command, function_set);
	hardware_delay(1);
	LCD_Write(command, lcd_on); // Display on
	hardware_delay(1);
	LCD_Write(command, lcd_clear); // Display clear
	hardware_delay(1);				
	LCD_Write(command, dec_mode); // Entry mode set
	hardware_delay(1);
}

void LCD_Print(uint8_t byte_count, uint8_t *array)
{
	uint8_t i;
	for(i = 0; i < byte_count; i++) {
		hardware_delay(500);
		LCD_Write(1, *(array + i));
	}
}
	