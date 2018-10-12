#ifndef _SD_H
#define _SD_H

#include "main.h"

#define ILLEGAL_COMMAND 0xE0
#define SD_INIT_ERROR 0xE1
#define COMM_ERROR 0xE3
#define DATA_ERROR 0xE4

// SD card commands 
#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD55 55
#define CMD58 58
#define ACMD41 41

uint8_t send_command(uint8_t command, uint32_t argument);
uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array);
uint8_t SD_card_init(void);
uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array);

#endif