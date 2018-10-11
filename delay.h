#ifndef _DELAY_H
#define _DELAY_H

#include "main.h"

#define TIMER_RELOAD (65536-((OSC_FREQ)/(1020*OSC_PER_INST)))
#define TIMER_RELOAD_THx (TIMER_RELOAD>>8)
#define TIMER_RELOAD_TLx (TIMER_RELOAD&0xFF)

void delay(uint16_t duration);

#endif
