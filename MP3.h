#ifndef _TRAFFIC_LIGHT_H
#define _TRAFFIC_LIGHT_H

#include "main.h"

// possible system states
typedef enum {
    DATA_IDLE_1,
    DATA_SEND_1,
    LOAD_BUFFER_1,
    FIND_CLUSTER_1,
    DATA_IDLE_2,
    DATA_SEND_2,
    LOAD_BUFFER_2,
    FIND_CLUSTER_2
} states_t;

// struct instance representing the system state
states_t SYSTEM_STATE = DATA_SEND_1;
uint32_t TIMER = 0, ALT_TIMER = 0;

void play_music_isr(void);

#endif