/*
	CpE 5160 Experiment 4
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

// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

extern uint32_t idata FirstRootDirSec_g;

// SD card data block
uint8_t xdata block_data_g[512];

void main(void) {
    uint8_t status;
    uint16_t num_entries, entry_num;
    uint32_t entry, sec_num;
    
    AUXR = 0x0C; // Make all of XRAM available
    
    if (OSC_PER_INST == 6) {
        CKCON0 = 0x01; // Set x2 clock mode
    }
    else {
        CKCON0 = 0x00; // Set standard clock mode
    }
	
	// Initialize UART
	UART_init(9600);
    
	// Initialize SPI at 400 KHz
    SPI_master_init(400000);
    
    // Initialize SD card
    printf("SD Card Initialization ... \n\r");
    status = SD_card_init();

    if(status ==  NO_ERROR) {
        green = 0;
    }
    else { // pause program to allow error message to be read
        while(1);
    }
   
    // Set SPI clock to 25 MHz
    SPI_master_init(25000000UL);
    
    // mount SD card
    mount_drive();
    
    // start at first root directory sector
    sec_num = FirstRootDirSec_g;
   
    // Super Loop
    while (1) {
        // list entries
        num_entries = Print_Directory(sec_num, &block_data_g);
        
        // Get block number from user
        printf("\nEnter selection: ");
        entry_num = long_serial_input();
        printf("entry_num: %2.2bX%2.2bX\n", entry_num, entry_num << 8);
        
        // if entry is greater than number of entries, prompt user again
        if (entry_num > num_entries) {
            continue;
        }
        
        entry = Read_Dir_Entry(sec_num, entry_num, &block_data_g);
        printf("Entry: 0x%2.2bX%2.2bX%2.2bX%2.2bX\n", entry, entry << 8, entry << 16, entry << 24);
        
        if ((entry >> 31) == 1) { // if error bit set
            red = 0;
            break;
        }
        
        if ((entry >> 28) == 1) { // if directory
            sec_num = First_Sector(entry & 0x0FFFFFFF);
            printf("Sector: 0x%2.2bX%2.2bX%2.2bX%2.2bX\n", sec_num, sec_num << 8, sec_num << 16, sec_num << 24);
        }
        else { // if file
            Open_File(entry & 0x0FFFFFFF, &block_data_g);
        }
    }
    
    while (1);
}