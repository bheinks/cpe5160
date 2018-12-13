#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "Long_Serial_In.h"
#include "UART.h"

/***********************************************************************
DESC:     Waits for user input of a long integer as ASCII characters
          
INPUT: Nothing
RETURNS: unsigned long
CAUTION: UART must be initialized first
************************************************************************/

uint32_t long_serial_input(void) {
    uint8_t index, input, input_values[11];    
    uint32_t output_value = 0xFFFFFFFF;

    for(index = 0; index < 11; index++) {
        input_values[index] = 0;
    }

    index = 0;
    do {
        input = UART_receive();

        if((input >= 0x30) && (input <= 0x39)) {
          input_values[index] = input;
          index++;
          putchar(input);
        }
        else if ((input == BS)||(input == DEL)) {  // Putty uses DEL (127) or BS (0x08) for backspace
            index--;
            input_values[index] = 0;
            UART_transmit(BS);
            UART_transmit(space);
            UART_transmit(BS);
        }
        else if ((input == CR) || (input == LF)) {
            output_value = atol(input_values);
            UART_transmit(CR);
            UART_transmit(LF);
        }
        else {
            input = 0;
        }
        if(index == 10) {
            output_value = atol(input_values);
            UART_transmit(CR);
            UART_transmit(LF);
        }
    } while(output_value == 0xFFFFFFFF);

    return output_value;
}