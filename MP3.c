#include "MP3.h"
#include "PORT.h"
#include "SPI.h"
#include "Directory_Functions.h"

#define MP3_TIMEOUT 11
#define MP3_RELOAD (65536-(OSC_FREQ*MP3_TIMEOUT)/(OSC_PER_INST*1000))
#define MP3_RELOAD_H (MP3_RELOAD/256)
#define MP3_RELOAD_L (MP3_RELOAD%256)

// global variables
extern uint8_t xdata BUFFER_1[512], BUFFER_2[512];
extern uint8_t idata SecPerClus_g;
extern uint16_t idata BytesPerSec_g;
extern bit idata PLAYING, PAUSE;

states_t SYSTEM_STATE;
uint16_t idata TIME = 0, BUFFER_1_PROGRESS = 0, BUFFER_2_PROGRESS = 0;
uint32_t idata CURRENT_CLUSTER_NUM, CURRENT_SECTOR_NUM;

// Function to set lights based on input numbers
void set_lights(bit green, bit amber, bit yellow, bit red, bit blue) {
    GREENLED = ~green;
    AMBERLED = ~amber;
    YELLOWLED = ~yellow;
    REDLED = ~red;
    BLUELED = ~blue;
}

void MP3_clock_reset(void) {
    TR1 = 0; // stop timer for reload
    
    TH1 = MP3_RELOAD_H;
    TL1 = MP3_RELOAD_L;

    TF1 = 0; // clear overflow flag
    TR1 = 1; // start timer 0
}

bit end_of_cluster(void) {
    return (CURRENT_SECTOR_NUM - first_sector(CURRENT_CLUSTER_NUM)) == SecPerClus_g;
}

// Function to control traffic light system state
void play_music_isr(void) interrupt TIMER_2_OVERFLOW {
    uint8_t rec_value;
    
    TF2 = 0; // clear interrupt flag
    TIME++;

    /*
    --Notes on switch state--
    TIMER reset before switching to any state that requires the timer
    FLASH_TIMER reset before any state that requires the flash timer
    No waiting in any state to avoid delaying past system tick
    */
    
    switch (SYSTEM_STATE) {
        case DATA_IDLE_1:
            set_lights(0, 1, 0, 0, 0);
        
            if (!DATA_REQ) {
                SYSTEM_STATE = DATA_SEND_1;
            }
        
            break;
        case DATA_SEND_1:
            set_lights(0, 0, 1, 0, 0);
            while(PAUSE){ //Pause if pause button pressed
                if(SW3 == 0){
                    PAUSE = 0;
                }
            }                
            MP3_clock_reset();

            // send data to the STA01 as fast as possible
            BIT_EN = 1;
            for(;BUFFER_1_PROGRESS < 512 && !DATA_REQ && !TF1; ++BUFFER_1_PROGRESS) {
                SPI_transfer(BUFFER_1[BUFFER_1_PROGRESS], &rec_value);
            }
            BIT_EN = 0;

            // if data request pin is inactive and buffer 2 has not sent all data
            if (DATA_REQ && BUFFER_2_PROGRESS != 512) {
                SYSTEM_STATE = DATA_IDLE_1;
            }
            // if data request pin is inactive and buffer 2 has sent all data
            else if (DATA_REQ && BUFFER_2_PROGRESS == 512) {
                if (end_of_cluster()) {
                    SYSTEM_STATE = FIND_CLUSTER_2;
                }
                else {
                    SYSTEM_STATE = LOAD_BUFFER_2;
                }
            }
            // if both buffer 1 and buffer 2 have sent all data
            else if (BUFFER_1_PROGRESS == 512 && BUFFER_2_PROGRESS == 512) {
                if (end_of_cluster()){
                    SYSTEM_STATE = FIND_CLUSTER_2;
                }
                else {
                    SYSTEM_STATE = LOAD_BUFFER_2;
                }
            }
            // if buffer 1 has sent all data but buffer 2 has not
            else if (BUFFER_1_PROGRESS == 512 && BUFFER_2_PROGRESS != 512) {
                SYSTEM_STATE = DATA_SEND_2;
            }
            break;
        case FIND_CLUSTER_2:
            set_lights(0, 0, 0, 1, 0);
                    
            // calculate next cluster
            CURRENT_CLUSTER_NUM = find_next_cluster(CURRENT_CLUSTER_NUM, &BUFFER_2);
            if (CURRENT_CLUSTER_NUM == 0x0FFFFFFF){
                EA = 0; // Disable Interrupts
                PLAYING = 0;
                break;
            }
            // update some shit with that number
            CURRENT_SECTOR_NUM = first_sector(CURRENT_CLUSTER_NUM);

            SYSTEM_STATE = DATA_IDLE_1;
        
            break;
        case LOAD_BUFFER_2:
            set_lights(0, 0, 0, 0, 1);
        
            // Load buffer 2 with data from SD card
            read_sector(CURRENT_SECTOR_NUM, BytesPerSec_g, &BUFFER_2);
            CURRENT_SECTOR_NUM++;

            BUFFER_2_PROGRESS = 0;
            
            SYSTEM_STATE = DATA_IDLE_1; 
        
            break;
        case DATA_IDLE_2:
            set_lights(1, 1, 0, 0, 0);
        
            if (!DATA_REQ) {
                SYSTEM_STATE = DATA_SEND_2;
            }
        
            break;
        case DATA_SEND_2:
            set_lights(1, 0, 1, 0, 0);
        
            while(PAUSE){ //Pause if pause button pressed
                if(SW3 == 0){
                    PAUSE = 0;
                }
            }   
        
            MP3_clock_reset();
            
            // send data to the STA01 as fast as possible
            BIT_EN = 1;
            for(; BUFFER_2_PROGRESS < 512 && !DATA_REQ && !TF1; ++BUFFER_2_PROGRESS) {
                SPI_transfer(BUFFER_2[BUFFER_2_PROGRESS], &rec_value);
            }
            BIT_EN = 0;
            
            // if data request pin is inactive and buffer 1 has not sent all data
            if (DATA_REQ && BUFFER_1_PROGRESS != 512) {
                SYSTEM_STATE = DATA_IDLE_2;
            }
            // if data request pin is inactive and buffer 1 has sent all data
            else if (DATA_REQ && BUFFER_1_PROGRESS == 512) {
                if (end_of_cluster()){
                    SYSTEM_STATE = FIND_CLUSTER_1;
                }
                else {
                    SYSTEM_STATE = LOAD_BUFFER_1;
                }
            }
            // if both buffer 2 and buffer 1 have sent all data
            else if (BUFFER_2_PROGRESS == 512 && BUFFER_1_PROGRESS == 512) {
                if (end_of_cluster()){
                    SYSTEM_STATE = FIND_CLUSTER_1;
                }
                else {
                    SYSTEM_STATE = LOAD_BUFFER_1;
                }
            }
            // if buffer 2 has sent all data but buffer 1 has not
            else if (BUFFER_2_PROGRESS == 512 && BUFFER_1_PROGRESS != 512) {
                SYSTEM_STATE = DATA_SEND_1;
            }
            
            break;
        case FIND_CLUSTER_1:
            set_lights(1, 0, 0, 1, 0);
        
            // calculate next cluster
            CURRENT_CLUSTER_NUM = find_next_cluster(CURRENT_CLUSTER_NUM, &BUFFER_1);
            if (CURRENT_CLUSTER_NUM == 0x0FFFFFFF){
                EA = 0; // Disable Interrupts
                PLAYING = 0;
                break;
            }                
            // update some shit with that number
            CURRENT_SECTOR_NUM = first_sector(CURRENT_CLUSTER_NUM);

            SYSTEM_STATE = DATA_IDLE_2;
            
            break;
        case LOAD_BUFFER_1:
            set_lights(1, 0, 0, 0, 1);

            // Load buffer 1 with data from SD card
            read_sector(CURRENT_SECTOR_NUM, BytesPerSec_g, &BUFFER_1);
            CURRENT_SECTOR_NUM++;

            BUFFER_1_PROGRESS = 0;

            SYSTEM_STATE = DATA_IDLE_2;
            
            break;
    }
}