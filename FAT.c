#include <stdio.h>

// local includes
#include "FAT.h"
#include "PORT.h"
#include "read_sector.h"


// LEDs
sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

uint8_t read8 (uint16_t offset, uint8_t * array_name){
   uint8_t idata ret;

   ret = 0;
   offset&=0x1FF; 
 
   ret=*(array_name + offset);
   return ret;
}

uint16_t read16 (uint16_t offset, uint8_t * array_name){
   uint16_t idata ret;
   uint8_t idata index, temp;

   offset&=0x1FF;  
   ret=0;


	for (index = 0; index < 3; index++){
		temp = *(array_name  +  offset  +  (2-index));
		ret = ret << 8;
		ret = ret | temp;
	}

   return ret;
}

uint32_t read32 (uint16_t offset, uint8_t * array_name){
   uint32_t idata ret;
   uint8_t idata temp, index;

   ret=0;
   offset&=0x1FF;  

   for(index=0;index<4;index++){   
       temp=*(array_name + offset + (3-index));
       ret=ret<<8;
       ret|=temp;
   }
   return ret;
}

uint8_t mount_drive(void) {
    uint32_t BPB_sector = 0;
    uint8_t xdata block_data[512];
    
    read_sector(0, 512, &block_data);
    printf("First byte: %2.2bX\n", block_data[0]);
    
    if ((block_data[0] != 0xEB) && (block_data[0] != 0xE9)) {
        printf("BPB not at 0\n");
        BPB_sector = read32(0x01C6, block_data);
        read_sector(BPB_sector, 512, &block_data);
    }
    
    if ((block_data[0] != 0xEB) && (block_data[0] != 0xE9)) {
        return BPB_NOT_FOUND;
    }
    
    printf("First byte: %2.2bX\n", block_data[0]);
    
    return NO_ERROR;
}
