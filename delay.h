#ifndef _DELAY_H
#define _DELAY_H

#include "main.h"

#define TIMER_RELOAD ((uint16_t)(65536-(OSC_FREQ/(OSC_PER_INST*1020UL))))
#define TIMER_RELOAD_H (TIMER_RELOAD/256)
#define TIMER_RELOAD_L (TIMER_RELOAD%256)

void delay(uint16_t duration);

#endif