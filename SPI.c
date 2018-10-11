#include "SPI.h"

uint8_t SPI_master_init(uint32_t clock_freq) {
    uint8_t divider, return_value;
    
    divider = (OSC_FREQ * 6) / (OSC_PER_INST * clock_freq);
    return_value = 0;
    
    if (divider < 2) {
        SPCON = 0x70 | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 4) {
        SPCON = 0x71 | (CPOL << 3) | (CPHA << 2);
    }
    else {
        return_value = CLOCK_RATE_ERROR;
    }
    
    return return_value;
}

uint8_t SPI_transfer(uint8_t send_value, uint8_t * rec_value) {
    uint8_t error_flag, status;
    uint16_t timeout;
    
    SPDAT = send_value;
    
    timeout = 0;
    do {
        status = SPSTA;
        timeout++;
    } while (((status & 0xF0) == 0) && (timeout != 0));
    
    if (timeout == 0) {
        error_flag = TIMEOUT_ERROR;
        *rec_value = 0xFF;
    }
    else if ((status & 0x70) != 0) {
        error_flag = SPI_ERROR;
        *rec_value = 0xFF;
    }
    else {
        error_flag = NO_ERROR;
        *rec_value = SPDAT;
    }
    
    return error_flag;
}