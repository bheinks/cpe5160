#include "main.h"
#include "PORT.h"
#include "delay.h"
#include "LCD.h"

void LCD_write(bit reg_select, uint8_t lcd_data) {
    RS = reg_select;
    E = 1;
    P0 = lcd_data;
    E = 0;
    P0 = 0xFF;
    RS = 1;
}

/*void LCD_write(bit RegSelect, uint8_t message) {
  RS=RegSelect;
  E=1;
  P0=message;   
  E=0;
  P0=0xFF;
}*/

void LCD_init(void) {
    // 8-bit interface, 2 lines, 5x8 font
    LCD_write(COMMAND, FUNCTION_SET|TWO_LINE|SET_5x8); 
    delay(5);
    LCD_write(COMMAND, FUNCTION_SET|TWO_LINE|SET_5x8); 
    delay(1);
    LCD_write(COMMAND, FUNCTION_SET|TWO_LINE|SET_5x8); 
    delay(1);
    LCD_write(COMMAND, FUNCTION_SET|TWO_LINE|SET_5x8); 
    delay(1);

    // display on, cursor on and not blinking
    LCD_write(COMMAND, DISPLAY_CONTROL|DISPLAY_ON|CURSOR_OFF|BLINK_OFF);
    delay(1);

    // clear display
    LCD_write(COMMAND, CLEAR_DISPLAY);
    delay(2); // wait until complete

    // DDRAM address increments with no scrolling
    LCD_write(COMMAND, ENTRY_MODE|SHIFT_RIGHT|CURSOR);
    delay(1);
}

/*void LCD_Init(void) {
	hardware_delay(30);
	LCD_write(command, function_set); // Function set x4
	hardware_delay(5);										
	LCD_write(command, function_set);
	hardware_delay(1);								
	LCD_write(command, function_set);
	hardware_delay(1);
	LCD_write(command, function_set);
	hardware_delay(1);
	LCD_write(command, lcd_on); // Display on
	hardware_delay(1);
	LCD_write(command, lcd_clear); // Display clear
	hardware_delay(1);				
	LCD_write(command, dec_mode); // Entry mode set
	hardware_delay(1);
}*/

void LCD_print(uint8_t address, uint8_t num_bytes, uint8_t *string_p) {
    uint8_t character, index;

    if (address != NO_ADDR_CHANGE) {
        LCD_write(COMMAND, SET_DDRAM_ADDR|address);
        delay(1);
    }

    if (num_bytes == 0) {
        index = 0;
        character = *(string_p + index);

        while (character != 0) {
            LCD_write(DDRAM, character);
            delay(1);
            index++;
            character = *(string_p + index);
        }
    }
    else {
        for(index = 0; index < num_bytes; ++index) {
            character = *(string_p + index);
            LCD_write(DDRAM, character);
            delay(1);
        }
    }
}

/*void LCD_print(uint8_t byte_count, uint8_t *array)
{
	uint8_t i;
	for(i = 0; i < byte_count; i++) {
		hardware_delay(500);
		LCD_write(1, *(array + i));
	}
}*/
