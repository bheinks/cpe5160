#include <stdio.h>
#include "STA013.h"
#include "I2C.h"
#include "PORT.h"
#include "delay.h"

uint8_t STA013_config(uint8_t * config_array);

sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

// import labels
extern uint8_t code CONFIG;
extern uint8_t code CONFIG2;
extern uint8_t code CONFIG3;

uint8_t STA013_init(void) {
    STA013_config(&CONFIG);
    delay(1);
    STA013_config(&CONFIG2);
    delay(1);
    STA013_config(&CONFIG3);
    
    return 0;
}

uint8_t STA013_config(uint8_t * config_array) {
    uint8_t send_array[1], internal_addr, timeout, status;
    uint16_t idata i = 0;
    
    internal_addr = config_array[i];
    i++;
    send_array[0] = config_array[i];
    i++;
    
    timeout = 50;
    while ((internal_addr != 0xFF) && (timeout != 0)) {
        timeout = 50;
        
        do {
            status = I2C_write(0x43, internal_addr, 1, 1, send_array);
            --timeout;
        } while ((status != NO_ERROR) && (timeout != 0));
    
        internal_addr = config_array[i];
        ++i;
        
        send_array[0] = config_array[i];
        ++i;
    }
    printf("Config sent...\n");
    
    return status;
}