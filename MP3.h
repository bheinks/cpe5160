#ifndef _MP3_H
#define _MP3_H

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

void play_music_isr(void);

#endif