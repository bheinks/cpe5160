#ifndef _TRAFFIC_LIGHT_H
#define _TRAFFIC_LIGHT_H

#include "main.h"

// possible system states
typedef enum {
    MAIN_GREEN,
    MAIN_YELLOW_CROSS,
    MAIN_YELLOW_SIDE,
    MAIN_RED_CROSS,
    MAIN_RED_SIDE,
    SIDE_BUTTON_RELEASE,
    SIDE_RED,
    SIDE_GREEN,
    SIDE_YELLOW,
    WALK,
    NO_WALK_ON,
    NO_WALK_OFF
} states_t;

// struct instance representing the system state
states_t SYSTEM_STATE = MAIN_GREEN;
uint32_t TIMER = 0, FLASH_TIMER = 0;

void traffic_light_isr(void);

#endif