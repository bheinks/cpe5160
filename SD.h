#ifndef _SD_H
#define _SD_H

#include "main.h"

#define ILLEGAL_COMMAND 0xE0

uint8_t send_command(uint8_t command, uint32_t argument);

#endif