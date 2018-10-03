#ifndef _LCD_H
#define _LCD_H

#include "Main.h"
#include "PORT.h"

// Commands
#define command 0
#define set_address 0x80
#define line1 0x00
#define line2 0x40
#define function_set 0x3C // 2 line mode and display on

// LCD commands
#define lcd_on 0x0C
#define lcd_off 0x08 // Turns off any display function
#define lcd_clear 0x01
#define cursor_on 0x0A
#define blink_on 0x09

// Entry Mode
#define dec_mode 0x06 // Right to left
#define inc_mode 0x04 // Left to right
#define shift_on 0x05


void LCD_Write(bit RegSelect, uint8_t message);
void LCD_Init(void);
void LCD_Print(uint8_t byte_count, uint8_t *array);

#endif