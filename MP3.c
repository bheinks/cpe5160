#include "MP3.h"
#include "PORT.h"

// Function to set lights based on input numbers
void set_lights(bit green, bit amber, bit yellow, bit red, bit blue) {
    GREENLED = ~green;
    AMBERLED = ~amber;
    YELLOWLED = ~yellow;
    REDLED = ~red;
    BLUELED = ~blue;
}

// Function to control traffic light system state
void play_music_isr(void) interrupt TIMER_2_OVERFLOW {
    TF2 = 0; // clear interrupt flag
    TIMER++; // increment timer
    ALT_TIMER++; // increment alternate timer

    /*
    --Notes on switch state--
    TIMER reset before switching to any state that requires the timer
    FLASH_TIMER reset before any state that requires the flash timer
    No waiting in any state to avoid delaying past system tick
    */
    
    switch (SYSTEM_STATE) {
        case DATA_IDLE_1:
            set_lights(0, 1, 0, 0, 0);
        
            //if (1 /* DR-active */) {
            //    SYSTEM_STATE = DATA_SEND_1;
            //}
        
            break;
        case DATA_SEND_1:
            set_lights(0, 0, 1, 0, 0);
        
            /* send data to the STA01 as fast as possible */
        
            if (1 /* DR-inactive & Buf2-empty */) {
                SYSTEM_STATE = DATA_IDLE_1;
            }
            
            else if (1 /* DR-inactive & Buf2 not empty */) {
                if (1 /* Cluster is empty */  ){
                    SYSTEM_STATE = FIND_CLUSTER_2;
                    break;
                }
                SYSTEM_STATE = LOAD_BUFFER_2;
            }
            
            else if (1 /* Buf1 empty & Buf2-empty */) {
                if (1 /* Cluster is empty */  ){
                    SYSTEM_STATE = FIND_CLUSTER_2;
                    break;
                }
                SYSTEM_STATE = LOAD_BUFFER_2;
            }
            
            else if (1 /* Buf1 empty & Buf2 not empty */) {
                SYSTEM_STATE = DATA_SEND_2;
            }            
        
            break;
        case FIND_CLUSTER_2:
            set_lights(0, 0, 0, 1, 0);
        
            /* calculate next cluster */
            /* update some shit with that number */
            SYSTEM_STATE = DATA_IDLE_1;
            
        
            break;
        case LOAD_BUFFER_2:
            set_lights(0, 0, 0, 0, 1);
        
            /* Load buffer 2 with data from SD card */
                SYSTEM_STATE = DATA_IDLE_1; 
            
        
            break;
        case DATA_IDLE_2:
            set_lights(1, 1, 0, 0, 0);
        
            if (1 /* DR-active */) {
                SYSTEM_STATE = DATA_SEND_2;
            }
        
            break;
        case DATA_SEND_2:
            set_lights(1, 0, 1, 0, 0);
            
            /* send data to the STA01 as fast as possible */
        
            if (1 /* DR-inactive & Buf1-empty */) {
                SYSTEM_STATE = DATA_IDLE_2;
            }
            
            else if (1 /* DR-inactive & Buf1 not empty */) {
                if (1 /* Cluster is empty */  ){
                    SYSTEM_STATE = FIND_CLUSTER_1;
                    break;
                }
                SYSTEM_STATE = LOAD_BUFFER_1;
            }
            
            else if (1/* Buf2 empty & Buf1-empty */) {
                if (1 /* Cluster is empty */  ){
                    SYSTEM_STATE = FIND_CLUSTER_1;
                    break;
                }
                SYSTEM_STATE = LOAD_BUFFER_1;
            }
            
            else if (1/* Buf2 empty & Buf1 not empty */) {
                SYSTEM_STATE = DATA_SEND_2;
            }
            
            break;
        case FIND_CLUSTER_1:
            set_lights(1, 0, 0, 1, 0);
        
            /* calculate next cluster */
            /* update some shit with that number */
            SYSTEM_STATE = DATA_IDLE_2;
            
            
            break;
        case LOAD_BUFFER_1:
            set_lights(1, 0, 0, 0, 1);
        
            /* Load buffer 2 with data from SD card */
            SYSTEM_STATE = DATA_IDLE_2;
            
            break;
        
    }
}