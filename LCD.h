#ifndef _LCD_H
#define _LCD_H

#include "main.h"

// commands
#define COMMAND 0
#define DDRAM 1

// lines and fonts
#define FUNCTION_SET 0x30
#define ONE_LINE 0x00
#define TWO_LINE 0x08
#define SET_5x8 0x00
#define SET_5x11 0x40

// display, cursor and blink
#define DISPLAY_CONTROL 0x08
#define DISPLAY_OFF 0x00
#define DISPLAY_ON 0x40
#define CURSOR_OFF 0x00
#define CURSOR_ON 0x20
#define BLINK_OFF 0x00
#define BLINK_ON 0x01

// shift
#define DISPLAY_MODE 0x10
#define SHIFT_RIGHT 0x08
#define SHIFT_LEFT 0x00
#define SHIFT_CURSOR 0x00
#define SHIFT_DISPLAY 0x04

#define SET_CG_ADDR 0x40
#define SET_DDRAM_ADDR 0x80
#define LINE1 0x00
#define TAB1 0x07
#define LINE2 0x40
#define TAB2 0x47
#define NO_ADDR_CHANGE 0xFF

#define ENTRY_MODE 0x04
#define INC_RIGHT 0x02
#define DEC_LEFT 0x00
#define CURSOR 0x00
#define DISPLAY 0x01

#define RETURN_HOME 0x02
#define CLEAR_DISPLAY 0x01

void LCD_write(bit reg_select, uint8_t lcd_data);
void LCD_init(void);
void LCD_print(uint8_t address, uint8_t num_bytes, uint8_t * string_p);

#endif