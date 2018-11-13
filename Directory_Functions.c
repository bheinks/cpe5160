#include "AT89C51RC2.h"
#include <stdio.h>
#include "main.h"
#include "PORT.H"
#include "UART.h"
#include "SPI.h"
#include "SD.h"
#include "Directory_Functions.h"
#include "print_bytes.h"
#include "read_sector.h"

// global variables
uint32_t idata FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
uint16_t idata BytesPerSec_g;
uint8_t idata SecPerClus_g, FATtype_g, BytesPerSecShift_g, FATshift_g;

uint32_t read(uint16_t offset, uint8_t * array_name, uint8_t num_bytes){
    uint32_t idata ret = 0;
    uint8_t idata i, temp;

    offset &= 0x1FF;
    
    for (i = 0; i < num_bytes; i++){
        temp = *(array_name + offset + ((num_bytes-1) - i));
        ret = ret << 8;
        ret = ret | temp;
    }

    return ret;
}

uint8_t mount_drive(void) {
    uint32_t BPB_sector = 0;
    uint8_t xdata block_data[512];
    
    read_sector(0, 512, &block_data);
    
    if ((block_data[0] != 0xEB) && (block_data[0] != 0xE9)) {
        BPB_sector = read(0x01C6, block_data, 4);
        read_sector(BPB_sector, 512, &block_data);
    }

    if ((block_data[0] != 0xEB) && (block_data[0] != 0xE9)) {
        return BPB_NOT_FOUND;
    }
    
    printf("First byte: %2.2bX\n", block_data[0]);
    
    BytesPerSec_g = read(0x0B, block_data, 2);
    printf("BytesPerSec: 0x%2.2bX%2.2bX\n", BytesPerSec_g, BytesPerSec_g << 8);
    
    return NO_ERROR;
}



/***********************************************************************
DESC: Prints all short file name entries for a given directory 
INPUT: Starting Sector of the directory and the pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint16_t number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/



uint16_t  Print_Directory(uint32_t Sector_num, uint8_t xdata * array_in)
{ 
   uint32_t idata Sector, max_sectors;
   uint16_t idata i, entries;
   uint8_t temp8, j, attr, out_val, error_flag;
   uint8_t * values;

   values=array_in;
   entries=0;
   i=0;
   if (Sector_num<FirstDataSec_g)  // included for FAT16 compatibility
   { 
      max_sectors=RootDirSecs_g;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=SecPerClus_g;
   }
   Sector=Sector_num;
   error_flag=read_sector(Sector, BytesPerSec_g, values);
   if(error_flag==NO_ERROR)
   {
     do
     {
 
	    temp8=read(0+i,values, 8);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read(0x0b+i,values, 8);
		   	YELLOWLED=1;
		   if((attr&0x0E)==0)   // if hidden, system or Vol_ID bit is set do not print
		   {
		      entries++;
			  printf("%5d. ",entries);  // print entry number with a fixed width specifier
		      for(j=0;j<8;j++)
			  {
			     out_val=read(i+j,values, 8);   // print the 8 byte name
			     putchar(out_val);
			  }
              if((attr&0x10)==0x10)  // indicates directory
			  {
			     for(j=8;j<11;j++)
			     {
			        out_val=read(i+j,values, 8);
			        putchar(out_val);
			     }
			     printf("[DIR]\n");
			  }
			  else       // print a period and the three byte extension for a file
			  {
			     putchar(0x2E);       
			     for(j=8;j<11;j++)
			     {
			        out_val=read(i+j,values, 8);
			        putchar(out_val);
			     }
			     putchar(0x0d);
                 putchar(0x0a);
			  }
		    }

		}
		i=i+32;  // next entry

		if(i>510)
		{
		  Sector++;
          if((Sector-Sector_num)<max_sectors)
		  {
              error_flag=read_sector(Sector, BytesPerSec_g, values);
			  if(error_flag!=NO_ERROR)
			    {
			      entries=0;   // no entries found indicates disk read error
				  temp8=0;     // forces a function exit
			    }
			    i=0;
		  }
		  else
		  {
			  entries=entries|more_entries;  // set msb to indicate more entries in another cluster
			  temp8=0;                       // forces a function exit
		  }
		}
       
	  }while(temp8!=0);
	}
	else
	{
	   entries=0;    // no entries found indicates disk read error
	}
    return entries;
 }


/***********************************************************************
DESC: Uses the same method as Print_Directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32_t with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/

uint32_t Read_Dir_Entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in)
{ 
   uint32_t idata Sector, max_sectors, return_clus;
   uint16_t idata i, entries;
   uint8_t temp8, attr, error_flag;
   uint8_t * values;

   values=array_in;
   entries=0;
   i=0;
   return_clus=0;
   if (Sector_num<FirstDataSec_g)  // included for FAT16 compatibility
   { 
      max_sectors=RootDirSecs_g;   // maximum sectors in a FAT16 root directory
   }
   else
   {
      max_sectors=SecPerClus_g;
   }
   Sector=Sector_num;
   error_flag=read_sector(Sector, BytesPerSec_g, values);
   if(error_flag==NO_ERROR)
   {
     do
     {
        temp8=read(0+i,values, 8);  // read first byte to see if empty
        if((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read(0x0b+i,values, 8);
		   if((attr&0x0E)==0)    // if hidden do not print
		   {
		      entries++;
              if(entries==Entry)
              {
			    if(FATtype_g==FAT32)
                {
                   return_clus=read(21+i,values, 8);
				   return_clus&=0x0F;            // makes sure upper four bits are clear
				   return_clus=return_clus<<8;
                   return_clus|=read(20+i,values, 8);
                   return_clus=return_clus<<8;
                }
                return_clus|=read(27+i,values, 8);
			    return_clus=return_clus<<8;
                return_clus|=read(26+i,values, 8);
			    attr=read(0x0b+i,values, 8);
			    if(attr&0x10) return_clus|=directory_bit;
                temp8=0;    // forces a function exit
              }
              
		   }
        }
		i=i+32;  // next entry
		if(i>510)
		{
		   Sector++;
		   if((Sector-Sector_num)<max_sectors)
		   {
              error_flag=read_sector(Sector, BytesPerSec_g, values);
			  if(error_flag!=NO_ERROR)
			  {
			     return_clus=no_entry_found;
                 temp8=0; 
			  }
			  i=0;
		   }
		   else
		   {
			  temp8=0;                       // forces a function exit
		   }
		}
        
	 }while(temp8!=0);
   }
   else
   {
	 return_clus=no_entry_found;
   }
   if(return_clus==0) return_clus=no_entry_found;
   return return_clus;
}





