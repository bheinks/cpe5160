#include "main.h"
#include "PORT.h"
#include "delay.h"

void delay(uint16_t duration) {
    TMOD &= 0xF0; // clear all T0 bits (T1 left unchanged)
    TMOD |= 0x01; // set required T0 bits (T1 left unchanged)

    ET0 = 0; // no interrupts

    do {
        // values for 1ms delay
        TH0 = TIMER_RELOAD_H;
        TL0 = TIMER_RELOAD_L;

        TF0 = 0; // clear overflow flag
        TR0 = 1; // start timer 0

        while (TF0 == 0);

        TR0 = 0; // stop timer 0
        duration--;
    } while (duration > 0);
}
