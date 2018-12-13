#include <stdio.h>

// local imports
#include "AT89C51RC2.h"
#include "PORT.h"
#include "main.h"
#include "SD.h"
#include "FAT.h"

bit print_long_name(uint16_t entry, uint8_t entry_num, uint8_t * array_in);

// global variables
uint32_t idata FIRST_DATA_SEC, START_OF_FAT, FIRST_ROOT_DIR_SEC, ROOT_DIR_SECS;
uint16_t idata BYTES_PER_SEC;
uint8_t idata SEC_PER_CLUS, FAT_TYPE;

extern uint8_t xdata BUFFER_1[512];

uint32_t read(uint16_t offset, uint8_t * array_name, uint8_t num_bytes){
    uint32_t idata value = 0;
    uint8_t idata i, temp;

    offset &= 0x1FF;
    
    for (i = 0; i < num_bytes; ++i){
        temp = *(array_name + offset + ((num_bytes-1) - i));
        value <<= 8;
        value |= temp;
    }

    return value;
}

uint8_t read_sector(uint32_t sector_number, uint16_t sector_size, uint8_t * byte_array) {
    uint8_t error = NO_ERROR;
    
    nCS0 = 0;
    
    error = send_command(CMD17, sector_number);
    if (error == NO_ERROR) {
        error = read_block(sector_size, byte_array);
    }
    
    nCS0 = 1;
    
    if (error != NO_ERROR) {
        error = DISK_ERROR;
    }
    
    return error;
}

uint8_t mount_drive(void) {
    uint32_t idata BPB_sec = 0, total_sec, FAT_size, hidden_sec, root_clus, data_sec, num_clusters;
    uint16_t idata num_rsvd_sec, num_root_entries;
    uint8_t num_FATs;
    
    // read sector 0 from SD
    read_sector(0, 512, &BUFFER_1);
    
    // if starting byte isn't 0xEB or 0xE9, this is MBR
    if ((BUFFER_1[0] != 0xEB) && (BUFFER_1[0] != 0xE9)) {
        BPB_sec = read(0x01C6, BUFFER_1, 4);
        // read sector at BPB offset
        read_sector(BPB_sec, 512, &BUFFER_1);
    }

    if ((BUFFER_1[0] != 0xEB) && (BUFFER_1[0] != 0xE9)) {
        return BPB_NOT_FOUND;
    }
    
    BYTES_PER_SEC = read(0x0B, BUFFER_1, 2);
    SEC_PER_CLUS = read(0x0D, BUFFER_1, 1);
    num_rsvd_sec = read(0x0E, BUFFER_1, 2);
    num_FATs = read(0x10, BUFFER_1, 1);
    num_root_entries = read(0x11, BUFFER_1, 2);
    
    total_sec = read(0x13, BUFFER_1, 2);
    if (total_sec == 0) { // if FAT32
        total_sec = read(0x20, BUFFER_1, 4);
    }
    
    FAT_size = read(0x16, BUFFER_1, 2);
    if (FAT_size == 0) { // if FAT32
        FAT_size = read(0x24, BUFFER_1, 4);
    }
    
    hidden_sec = read(0x1C, BUFFER_1, 4);
    root_clus = read(0x2C, BUFFER_1, 4);
    
    ROOT_DIR_SECS = ((num_root_entries * 32) + (BYTES_PER_SEC - 1)) / BYTES_PER_SEC;
    data_sec = total_sec - (num_rsvd_sec + (num_FATs + FAT_size) + ROOT_DIR_SECS);
    num_clusters = data_sec / SEC_PER_CLUS;
    
    if (num_clusters >= 65525) {
        FAT_TYPE = FAT32;
    }
    else { // FAT12 or FAT16
        printf("FAT type not supported\n");
        return FS_NOT_SUPPORTED;
    }
    
    START_OF_FAT = num_rsvd_sec + hidden_sec;
    FIRST_DATA_SEC = num_rsvd_sec + (num_FATs * FAT_size) + ROOT_DIR_SECS + hidden_sec;
    FIRST_ROOT_DIR_SEC = ((root_clus - 2) * SEC_PER_CLUS) + FIRST_DATA_SEC;
    
    return NO_ERROR;
}

uint32_t first_sector(uint32_t cluster_num) {
    if (cluster_num == 0){
        return FIRST_ROOT_DIR_SEC;
    }
    
    return (((cluster_num - 2) * SEC_PER_CLUS) + FIRST_DATA_SEC);
}

bit print_long_name(uint16_t entry, uint8_t entry_num, uint8_t * array_in) {
    uint16_t i;
    uint8_t out_val = read(entry, array_in, 1);
    bit last_entry = 0;
    
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
DESC: Prints all short file name entries for a given directory 
INPUT: Starting Sector of the directory and the pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint16_t number of entries found in the directory
CAUTION: Supports FAT16, SD_shift must be set before using this function
************************************************************************/

uint16_t print_directory(uint32_t sector_num, uint8_t xdata * array_in) { 
    uint32_t idata sector = sector_num;
    uint16_t idata i = 0, entries = 0;
    uint8_t temp8, j, attr, out_val, error_flag;
    uint8_t * values = array_in;
   
    error_flag = read_sector(sector, BYTES_PER_SEC, values);
    if (error_flag == NO_ERROR) {
        do {
            temp8 = read(i, values, 1);  // read first byte to see if empty
            
            if ((temp8 != 0xE5) && (temp8 != 0x00)) {  
                attr = read(0x0b+i, values, 1);
                
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
                sector++;

                if ((sector - sector_num) < SEC_PER_CLUS) {
                    error_flag = read_sector(sector, BYTES_PER_SEC, values);
                    if (error_flag != NO_ERROR) {
                        entries = 0;   // no entries found indicates disk read error
                        temp8 = 0;     // forces a function exit
                    }
                    i = 0;
                }
                else {
                    entries = entries | MORE_ENTRIES;  // set msb to indicate more entries in another cluster
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

/***********************************************************************
DESC: Uses the same method as print_directory to locate short file names,
      but locates a specified entry and returns and cluster  
INPUT: Starting Sector of the directory, an entry number and a pointer to a 
block of memory in xdata that can be used to read blocks from the SD card
RETURNS: uint32_t with cluster in lower 28 bits.  Bit 28 set if this is 
         a directory entry, clear for a file.  Bit 31 set for error.
CAUTION: 
************************************************************************/

uint32_t read_dir_entry(uint32_t sector_num, uint16_t entry, uint8_t xdata * array_in, uint8_t * filename) { 
    uint32_t idata sector = sector_num, return_clus = 0;
    uint16_t idata i = 0, entries = 0;
    uint8_t temp8, attr, error_flag, j;
    uint8_t * values;

    values = array_in;
    error_flag = read_sector(sector, BYTES_PER_SEC, values);
    
    if (error_flag == NO_ERROR) {
        do {
            temp8 = read(0+i, values, 1); // read first byte to see if empty

            if ((temp8 != 0xE5) && (temp8 != 0x00)) {  
                attr = read(0x0b+i, values, 1);

                if ((attr&0x0E) == 0) { // if hidden do not print
                    entries++;
                    
                    for (j = 0; j < 8; j++) {
                        filename[j] = read(i+j, values, 1);
                    }
                    
                    if (entries == entry) {
                        if (FAT_TYPE == FAT32) {
                            return_clus = read(21+i, values, 1);
                            return_clus &= 0x0F;                // makes sure upper four bits are clear
                            return_clus <<= 8;
                            return_clus |= read(20+i, values, 1);
                            return_clus <<= 8;
                        }
                        
                        return_clus |= read(27+i, values, 1);
                        return_clus <<= 8;
                        return_clus |= read(26+i, values, 1);

                        attr = read(0x0b+i, values, 1);
                        if (attr&0x10) {
                            return_clus |= DIRECTORY_BIT;
                        }
                        
                        temp8 = 0; // forces a function exit
                    }
                }
            }
            i += 32; // next entry

            if (i > 510) {
                sector++;

                if ((sector - sector_num) < SEC_PER_CLUS) {
                    error_flag = read_sector(sector, BYTES_PER_SEC, values);
                      
                    if (error_flag != NO_ERROR) {
                        return_clus = NO_ENTRY_FOUND;
                        temp8=0; 
                    }
                    i = 0;
                }
                else {
                    temp8 = 0; // forces a function exit
                }
            }
        } while (temp8 != 0);
    }
    else {
        return_clus = NO_ENTRY_FOUND;
    }
    if (return_clus == 0) {
        return_clus = NO_ENTRY_FOUND;
    }
    
    return return_clus;
}

uint32_t find_next_cluster(uint32_t cluster_num, uint8_t xdata * array_name){
    read_sector(START_OF_FAT + ((cluster_num*4) / BYTES_PER_SEC), 512, array_name);
    return read((cluster_num*4) % BYTES_PER_SEC, array_name, 4) & 0x0FFFFFFF;
}