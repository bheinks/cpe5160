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
#include "read_sector.h"
#include "print_bytes.h"
#include "sEOS.h"

extern uint32_t idata FirstRootDirSec_g;

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
    
	// Initialize SPI at 400 KHz
    SPI_master_init(400000);
    
    // Initialize SD card
    SD_card_init();
   
    // Set SPI clock to 25 MHz
    SPI_master_init(25000000UL);
    
    // reset STA013
    STA013_RESET = 0;
	STA013_RESET = 1;
    
    // initialize STA013
    STA013_init();
    
    // mount SD card
    mount_drive();
    
    // initialize timer 1
    TMOD &= 0x0F; // clear all T1 bits (T0 left unchanged)
    TMOD |= 0x10; // set required T1 bits (T0 left unchanged)
    ET1 = 0; // no interrupts
}

void main(void) {
    uint16_t num_entries, entry_num;
    uint32_t entry, sec_num;
    
    // initialize system and peripherals
    system_init();
    
    // initialize sEOS interrupt at 12ms
    seos_init(12);
    
    // start at first root directory sector
    sec_num = FirstRootDirSec_g;
   
    // Super Loop
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
        
        entry = read_dir_entry(sec_num, entry_num, &BUFFER_1);
        
        if ((entry >> 31) == 1) { // if error bit set
            REDLED = 0;
            break;
        }
        
        if ((entry >> 28) == 1) { // if directory
            sec_num = first_sector(entry & 0x0FFFFFFF);
        }
        else { // if file
            CURRENT_CLUSTER_NUM = entry & 0x0FFFFFFF;
            CURRENT_SECTOR_NUM = first_sector(CURRENT_CLUSTER_NUM);
            read_sector(CURRENT_SECTOR_NUM, SecPerClus_g, &BUFFER_1);
            CURRENT_SECTOR_NUM++;
            read_sector(CURRENT_SECTOR_NUM, SecPerClus_g, &BUFFER_2);
            CURRENT_SECTOR_NUM++;
        }
        
        go_to_sleep();
    }
    
    while (1);
}
