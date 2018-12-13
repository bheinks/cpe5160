#ifndef _FAT_H
#define _FAT_H

#include "main.h"

//------- Public Constants  -----------------------------------------
#define FAT32 (4)
#define FAT16 (2)
#define FAT32_SHIFT (2)
#define FAT16_SHIFT (1)
#define DISK_ERROR (0xF0)
#define MORE_ENTRIES (0x8000)   
#define NO_ENTRY_FOUND (0x80000000)  // msb set to indicate error
#define DIRECTORY_BIT (0x10000000)  // lsb of first nibble (bit28)
#define BPB_NOT_FOUND 0xE9
#define FS_NOT_SUPPORTED 0xE8

// ------ Public function prototypes -------------------------------
uint32_t read(uint16_t offset, uint8_t * array_name, uint8_t num_bytes);
uint8_t read_sector(uint32_t sector_number, uint16_t sector_size, uint8_t * byte_array);
uint8_t mount_drive(void);
uint32_t first_sector(uint32_t cluster_num);
uint16_t print_directory(uint32_t sector_num, uint8_t xdata * array_in);
uint32_t read_dir_entry(uint32_t sector_num, uint16_t entry, uint8_t xdata * array_in, uint8_t * filename);
uint32_t find_next_cluster(uint32_t cluster_num, uint8_t xdata * array_name);

#endif