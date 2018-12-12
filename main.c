/*
	CpE 5160 Experiment 4 (w/ long file name bonus)
	Brett Heinkel
	Michael Proemsey
	Ian Piskulic
*/

#include <stdio.h>

// local includes
#include "main.h"
#include "PORT.h"
#include "UART.h"
#include "delay.h"
#include "LCD.h"
#include "SPI.h"
#include "Long_Serial_In.h"
#include "SD.h"
#include "I2C.h"
#include "STA013.h"
#include "Directory_Functions.h"
#include "print_bytes.h"
#include "sEOS.h"
#include "MP3.h"
#include "LCD.h"

extern uint32_t idata FirstRootDirSec_g, CURRENT_CLUSTER_NUM, CURRENT_SECTOR_NUM;
extern uint16_t idata TIME;
extern uint8_t idata SecPerClus_g;
extern states_t SYSTEM_STATE;

bit idata PLAYING, PAUSE;

// SD card data blocks
uint8_t xdata BUFFER_1[512];
uint8_t xdata BUFFER_2[512];

void system_init(void) {
    AUXR = 0x0C; // Make all of XRAM available
    
    if (OSC_PER_INST == 6) {
        CKCON0 = 0x01; // Set x2 clock mode
    }
    else {
        CKCON0 = 0x00; // Set standard clock mode
    }
	
	// Initialize UART
	UART_init(9600);
    delay(300);
    
    // initialize LCD
    LCD_init();
    
	// Initialize SPI at 400 KHz
    SPI_master_init(400000);
    
    // Initialize SD card
    SD_card_init();
   
    // Set SPI clock to 25 MHz
    SPI_master_init(25000000UL);
    
    // mount filesystem
    mount_drive();
    
    // reset and initialize STA013
    STA013_RESET = 0;
	STA013_RESET = 1;
    STA013_init();
    
    // initialize timer 1
    TMOD &= 0x0F; // clear all T1 bits (T0 left unchanged)
    TMOD |= 0x10; // set required T1 bits (T0 left unchanged)
    ET1 = 0; // no interrupts
}

void main(void) {
    uint8_t idata time_buffer[8], filename[8], seconds;
    uint16_t idata num_entries, entry_num;
    uint32_t idata entry, sec_num;
    
    // initialize system and peripherals
    system_init();
    
    // initialize sEOS interrupt at 12ms
    sEOS_init(12);
    
    // start at first root directory sector
    sec_num = FirstRootDirSec_g;
   
    // super loop
    while (1) {
        // list entries
        num_entries = print_directory(sec_num, &BUFFER_1);        
        
        // Get block number from user
        printf("\nEnter selection: ");
        entry_num = long_serial_input();
        
        // if entry is equal to 0 or greater than number of entries, prompt user again
        if ((entry_num == 0) || (entry_num > num_entries)) {
            continue;
        }
        
        entry = read_dir_entry(sec_num, entry_num, &BUFFER_1, &filename);
        
        if ((entry >> 31) == 1) { // if error bit set
            REDLED = 0;
            break;
        }
        
        if ((entry >> 28) == 1) { // if directory
            sec_num = first_sector(entry & 0x0FFFFFFF);
            continue;
        }
        else { // if file
            // load both buffers
            CURRENT_CLUSTER_NUM = entry & 0x0FFFFFFF;
            CURRENT_SECTOR_NUM = first_sector(CURRENT_CLUSTER_NUM);
            
            read_sector(CURRENT_SECTOR_NUM, SecPerClus_g, &BUFFER_1);
            CURRENT_SECTOR_NUM++;
            
            read_sector(CURRENT_SECTOR_NUM, SecPerClus_g, &BUFFER_2);
            CURRENT_SECTOR_NUM++;
            
            LCD_print(LINE1, 0, filename); // print song title to LCD
            
            // set up state
            PLAYING = 1;
            PAUSE = 0;
            SYSTEM_STATE = DATA_SEND_1;
            EA = 1; // Enable Interrupts
        }

        while(PLAYING && SW1){
            if(SW2 == 0){
                PAUSE = 1;
            }

            if ((TIME % 80) == 0) {
                /*
                    seconds = numSeconds % 60;
                    minutes = (numSeconds - seconds) / 60;
                */
                seconds = ((TIME*80) / 1000);
                sprintf(&time_buffer, "%d:%d", (seconds - (seconds%60) / 60), (seconds % 60));
                LCD_print(LINE2, 0, time_buffer);
            }

            go_to_sleep();
        }
        EA = 0;
    }
    
    while (1); // if you're here, ya dun goofed
}
