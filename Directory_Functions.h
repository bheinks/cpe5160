#ifndef _Directory_Func_H
#define _Directory_Func_H

#include "main.h"

//------- Public Constants  -----------------------------------------
#define FAT32 (4)
#define FAT16 (2)
#define FAT32_shift (2)
#define FAT16_shift (1)
#define Disk_Error (0xF0)
#define No_Disk_Error (0)
#define more_entries (0x8000)   
#define no_entry_found (0x80000000)  // msb set to indicate error
#define directory_bit  (0x10000000)  // lsb of first nibble (bit28)
#define BPB_NOT_FOUND 0xE9
#define FS_NOT_SUPPORTED 0xE8

// ------ Public function prototypes -------------------------------

uint32_t read(uint16_t offset, uint8_t * array_name, uint8_t num_bytes);

uint8_t mount_drive(void);

uint32_t First_Sector (uint32_t Cluster_Num);

uint16_t Print_Directory(uint32_t Sector_num, uint8_t xdata * array_in);

uint32_t Read_Dir_Entry(uint32_t Sector_num, uint16_t Entry, uint8_t xdata * array_in);

//uint32 Find_Next_Clus(uint32 Cluster_num, uint8 xdata * array_name);

//uint8 Open_File(uint32 Cluster, uint8 xdata * array_in);

#endif