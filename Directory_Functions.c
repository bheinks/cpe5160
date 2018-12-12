#include <stdio.h>

#include "AT89C51RC2.h"
#include "PORT.h"
#include "main.h"
#include "Directory_Functions.h"
#include "read_sector.h"

bit print_long_name(uint16_t entry, uint8_t entry_num, uint8_t * array_in);

// global variables
uint32_t idata FirstDataSec_g, StartofFAT_g, FirstRootDirSec_g, RootDirSecs_g;
uint16_t idata BytesPerSec_g;
uint8_t idata SecPerClus_g, FATtype_g, BytesPerSecShift_g, FATshift_g;

extern uint8_t xdata BUFFER_1[512];

uint32_t read(uint16_t offset, uint8_t * array_name, uint8_t num_bytes){
    uint32_t idata ret = 0;
    uint8_t idata i, temp;

    offset &= 0x1FF;
    
    for (i = 0; i < num_bytes; ++i){
        temp = *(array_name + offset + ((num_bytes-1) - i));
        ret = ret << 8;
        ret = ret | temp;
    }

    return ret;
}

uint8_t mount_drive(void) {
    uint32_t BPBSec = 0, TotalSec, FATSize, HiddenSec, RootClus, DataSec, CountofClusters;
    uint16_t idata RsvdSecCnt, RootEntryCnt;
    uint8_t NumFATs;
    
    // read sector 0 from SD
    read_sector(0, 512, &BUFFER_1);
    
    // if starting byte isn't 0xEB or 0xE9, this is MBR
    if ((BUFFER_1[0] != 0xEB) && (BUFFER_1[0] != 0xE9)) {
        BPBSec = read(0x01C6, BUFFER_1, 4);
        // read sector at BPB offset
        read_sector(BPBSec, 512, &BUFFER_1);
    }

    if ((BUFFER_1[0] != 0xEB) && (BUFFER_1[0] != 0xE9)) {
        return BPB_NOT_FOUND;
    }
    
    BytesPerSec_g = read(0x0B, BUFFER_1, 2);
    SecPerClus_g = read(0x0D, BUFFER_1, 1);
    RsvdSecCnt = read(0x0E, BUFFER_1, 2);
    NumFATs = read(0x10, BUFFER_1, 1);
    RootEntryCnt = read(0x11, BUFFER_1, 2);
    
    TotalSec = read(0x13, BUFFER_1, 2);
    if (TotalSec == 0) { // if FAT32
        TotalSec = read(0x20, BUFFER_1, 4);
    }
    
    FATSize = read(0x16, BUFFER_1, 2);
    if (FATSize == 0) { // if FAT32
        FATSize = read(0x24, BUFFER_1, 4);
    }
    
    HiddenSec = read(0x1C, BUFFER_1, 4);
    RootClus = read(0x2C, BUFFER_1, 4);
    
    RootDirSecs_g = ((RootEntryCnt * 32) + (BytesPerSec_g - 1)) / BytesPerSec_g;
    DataSec = TotalSec - (RsvdSecCnt + (NumFATs + FATSize) + RootDirSecs_g);
    CountofClusters = DataSec / SecPerClus_g;
    
    if (CountofClusters >= 65525) {
        FATtype_g = FAT32;
    }
    else { // FAT12 or FAT16
        printf("FAT type not supported\n");
        return FS_NOT_SUPPORTED;
    }
    
    StartofFAT_g = RsvdSecCnt + HiddenSec;
    FirstDataSec_g = RsvdSecCnt + (NumFATs * FATSize) + RootDirSecs_g + HiddenSec;
    FirstRootDirSec_g = ((RootClus - 2) * SecPerClus_g) + FirstDataSec_g;
    
    return NO_ERROR;
}

uint32_t first_sector(uint32_t Cluster_Num) {
    if (Cluster_Num == 0){
        return FirstRootDirSec_g;
    }
    
    return (((Cluster_Num - 2) * SecPerClus_g) + FirstDataSec_g);
}

/***********************************************************************
DESC: Prints all short file name entries for a given directory 
INPUT: Starting Sector of the directory and the pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint16_t number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/

uint16_t print_directory(uint32_t Sector_num, uint8_t xdata * array_in) { 
    uint32_t idata Sector, max_sectors;
    uint16_t idata i = 0, entries = 0;
    uint8_t temp8, j, attr, out_val, error_flag;
    uint8_t * values = array_in;

    if (Sector_num < FirstDataSec_g) {  // included for FAT16 compatibility 
        max_sectors = RootDirSecs_g;   // maximum sectors in a FAT16 root directory
    }
    else {
        max_sectors = SecPerClus_g;
    }
    Sector = Sector_num;
   
    error_flag = read_sector(Sector, BytesPerSec_g, values);
    if (error_flag==NO_ERROR) {
        do {
            temp8 = read(i, values, 1);  // read first byte to see if empty
            
            if ((temp8 != 0xE5) && (temp8 != 0x00)) {  
                attr = read(0x0b+i,values, 1);
                
                if ((attr&0x0E) == 0) {   // if hidden, system or Vol_ID bit is set do not print
                    entries++;
                    printf("%5d. ", entries);  // print entry number with a fixed width specifier
                   
                    // if no long file name exists, print short name
                    if (!print_long_name(i-32, 1, values)) {
                        for (j = 0; j < 8; j++) {
                            out_val = read(i+j, values, 1);   // print the 8 byte name
                            putchar(out_val);
                        }
                      
                        if ((attr&0x10) == 0x10) {  // indicates directory
                            for (j = 8; j < 11; ++j) {
                                out_val = read(i+j, values, 1);
                                putchar(out_val);
                            }
                            printf("[DIR]\n");
                        }
                        else {       // print a period and the three byte extension for a file
                            putchar(0x2E);
                            for (j = 8; j < 11; ++j) {
                                out_val = read(i+j, values, 1);
                                putchar(out_val);
                            }
                            putchar(0x0a);
                        }
                    }
                    else {
                        if ((attr&0x10) == 0x10) {  // indicates directory
                            for (j = 8; j < 11; ++j) {
                                out_val = read(i+j, values, 1);
                                putchar(out_val);
                            }

                            printf("\t[DIR]\n");
                        }
                        else {
                            putchar('\n');
                        }
                    }
                }
            }
            i += 32;  // next entry

            if (i > 510) {
                Sector++;
                if ((Sector-Sector_num) < max_sectors) {
                    error_flag = read_sector(Sector, BytesPerSec_g, values);
                    if (error_flag != NO_ERROR) {
                        entries = 0;   // no entries found indicates disk read error
                        temp8 = 0;     // forces a function exit
                    }
                    i = 0;
                }
                else {
                    entries = entries|more_entries;  // set msb to indicate more entries in another cluster
                    temp8 = 0;                       // forces a function exit
                }
            }
        } while (temp8 != 0);
	}
	else {
	   entries = 0;    // no entries found indicates disk read error
	}

    return entries;
}

bit print_long_name(uint16_t entry, uint8_t entry_num, uint8_t * array_in) {
    uint16_t i;
    uint8_t out_val;
    bit last_entry = 0;
    
    out_val = read(entry, array_in, 1);
    
    // if first byte doesn't equal entry number, this isn't a long file entry
    if ((out_val & 0x3F) != entry_num) {
        return 0;
    }
    // if first byte is masked with 0x40, this is the last long file entry
    else if ((out_val & 0x40) == 0x40) {
        last_entry = 1;
    }
    
    // read characters 1 through 5
    for (i = 0; i < 10; i += 2) {
        out_val = read(entry+1+i, array_in, 1);
        
        // return if null character
        if (out_val == 0) {
            return 1;
        }
        
        putchar(out_val);
    }
    
    // read characters 6 through 11
    for (i = 0; i < 12; i += 2) {
        out_val = read(entry+14+i, array_in, 1);

        if (out_val == 0) {
            return 1;
        }

        putchar(out_val);
    }
    
    // read characters 12 and 13
    for (i = 0; i < 4; i += 2) {
        out_val = read(entry+28+i, array_in, 1);
        
        if (out_val == 0) {
            return 1;
        }
        
        putchar(out_val);
    }
    
    // return last_entry (1) if set, otherwise recurse to next entry
    return (last_entry || print_long_name(entry-32, entry_num+1, array_in));
}

/***********************************************************************
DESC: Uses the same method as print_directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32_t with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/

uint32_t read_dir_entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in) { 
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
   error_flag = read_sector(Sector, BytesPerSec_g, values);
   if (error_flag == NO_ERROR) {
     do {
        temp8=read(0+i,values, 1);  // read first byte to see if empty
        if ((temp8!=0xE5)&&(temp8!=0x00))
	    {  
	       attr=read(0x0b+i,values, 1);
		   if ((attr&0x0E)==0)    // if hidden do not print
		   {
		      entries++;
              if (entries==Entry)
              {
			    if (FATtype_g==FAT32)
                {
                   return_clus=read(21+i,values, 1);
				   return_clus&=0x0F;            // makes sure upper four bits are clear
				   return_clus=return_clus<<8;
                   return_clus|=read(20+i,values, 1);
                   return_clus=return_clus<<8;
                }
                return_clus|=read(27+i,values, 1);
			    return_clus=return_clus<<8;
                return_clus|=read(26+i,values, 1);
			    attr=read(0x0b+i,values, 1);
			    if (attr&0x10) return_clus|=directory_bit;
                temp8=0;    // forces a function exit
              }
              
		   }
        }
		i=i+32;  // next entry
		if (i>510)
		{
		   Sector++;
		   if ((Sector-Sector_num)<max_sectors)
		   {
              error_flag=read_sector(Sector, BytesPerSec_g, values);
			  if (error_flag!=NO_ERROR)
			  {
			     return_clus=no_entry_found;
                 temp8=0; 
			  }
			  i=0;
		   }
		   else
		   {
			  temp8=0; // forces a function exit
		   }
		}
        
	 } while (temp8!=0);
   }
   else
   {
	 return_clus=no_entry_found;
   }
   if (return_clus==0) return_clus=no_entry_found;
   return return_clus;
}

uint32_t find_next_cluster(uint32_t Cluster_num, uint8_t xdata * array_name){
    read_sector(StartofFAT_g + ((Cluster_num*4) / BytesPerSec_g), 512, array_name);
    return read((Cluster_num*4) % BytesPerSec_g, array_name, 4) & 0x0FFFFFFF;
}

/* void open_file(uint32_t Cluster, uint8_t xdata * array_in) {
    uint8_t i, cont;
    uint32_t sec_num, clus_num = Cluster;
    
    do {
        sec_num = first_sector(clus_num);
        
        for (i = 0; i < SecPerClus_g; ++i) {
            read_sector(sec_num, BytesPerSec_g, array_in);
            print_memory(array_in, BytesPerSec_g);
            sec_num++;
            
            printf("\nContinue? [Y/n] ");
            cont = getchar();
            
            if (cont != '\n') {
                getchar(); // consume extra newline
                
                if ((cont != 'Y') && (cont != 'y')) {
                    return;
                }
            }
        }
        
        clus_num = find_next_cluster(clus_num, array_in);
    } while (1);
}n */