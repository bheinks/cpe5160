#ifndef _SD_H
#define _SD_H

#include "main.h"

#define ILLEGAL_COMMAND 0xE0
#define SD_INIT_ERROR 0xE1
#define COMM_ERROR 0xE3
#define DATA_ERROR 0xE4

#define no_errors (0)
#define timeout_error (0x81)
#define illegal_cmnd (0x82)
#define response_error (0x83)
#define data_error (0x84)
#define voltage_error (0x85)
#define card_inactive (0x86)
#define SPI_error (0x87)

// SD card commands 
#define CMD0 0
#define CMD8 8
#define CMD16 16
#define CMD17 17
#define CMD55 55
#define CMD58 58
#define ACMD41 41

// SD Card types
#define unknown (0xFF)
#define Ver2 (0x02)
#define Standard_Capacity (9)  // used to shift address for block to byte address conversion
#define High_Capacity (0)      // no shift when no conversion is required

uint8_t send_command(uint8_t command, uint32_t argument);
uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array);
uint8_t SD_card_init(void);
uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array);
void print_error(uint8_t error);

#endif