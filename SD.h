#ifndef _SD_H
#define _SD_H

#include "main.h"

#define ILLEGAL_COMMAND 0xE0
#define COMM_ERROR 0x83

uint8_t send_command(uint8_t command, uint32_t argument);
uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array);

#endif