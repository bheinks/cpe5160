/*
	CpE 5160 Experiment 6
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
#include "SD.h"
#include "STA013.h"
#include "FAT.h"
#include "sEOS.h"
#include "MP3.h"
#include "LCD.h"
#include "I2C.h"

extern uint32_t idata FIRST_ROOT_DIR_SEC, CURRENT_CLUSTER_NUM, CURRENT_SECTOR_NUM, TIME;
extern uint8_t idata SEC_PER_CLUS;
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
    STA013_init(DEVICE_ADDR);
    
    // initialize timer 1
    TMOD &= 0x0F; // clear all T1 bits (T0 left unchanged)
    TMOD |= 0x10; // set required T1 bits (T0 left unchanged)
    ET1 = 0; // no interrupts
}

void main(void) {
    uint8_t idata filename[8];
    uint16_t idata num_entries, entry_num;
    uint32_t idata entry, sec_num;

    // initialize system and peripherals
    system_init();

    // initialize sEOS interrupt
    sEOS_init(SEOS_INTERVAL);

    // start at first root directory sector
    sec_num = FIRST_ROOT_DIR_SEC;

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
            
            read_sector(CURRENT_SECTOR_NUM, SEC_PER_CLUS, &BUFFER_1);
            CURRENT_SECTOR_NUM++;
            
            read_sector(CURRENT_SECTOR_NUM, SEC_PER_CLUS, &BUFFER_2);
            CURRENT_SECTOR_NUM++;
            
            LCD_print(LINE1, 0, filename); // print song title to LCD
            
            // set up state
            PLAYING = 1;
            PAUSE = 0;
            SYSTEM_STATE = DATA_SEND_1;
            EA = 1; // enable interrupts
        }

        while(PLAYING && SW1) {
            // if pause button is pressed, pause
            if(SW2 == 0){
                PAUSE = 1;
            }

            go_to_sleep();
        }

        EA = 0; // disable interrupts
        TIME = 0; // reset TIME
        LCD_write(COMMAND, CLEAR_DISPLAY); // clear LCD
    }

    while (1); // if you're here, ya dun goofed
}