#include "STA013.h"
#include "I2C.h"
#include "delay.h"

uint8_t STA013_config(uint8_t device_addr, uint8_t * config_array);

// import labels
extern uint8_t code CONFIG;
extern uint8_t code CONFIG2;
extern uint8_t code CONFIG3;

uint8_t STA013_init(uint8_t device_addr) {
    STA013_config(device_addr, &CONFIG);
    delay(1);
    STA013_config(device_addr, &CONFIG2);
    delay(1);
    STA013_config(device_addr, &CONFIG3);
    
    return 0;
}

uint8_t STA013_config(uint8_t device_addr, uint8_t * config_array) {
    uint8_t idata send_array[1], internal_addr, timeout, status;
    uint16_t idata i = 0;
    
    do {
        timeout = 50;
        
        internal_addr = config_array[i];
        ++i;
        
        send_array[0] = config_array[i];
        ++i;
        
        do {
            status = I2C_write(device_addr, internal_addr, 1, 1, send_array);
            --timeout;
        } while ((status != NO_ERROR) && (timeout != 0));
    } while ((internal_addr != 0xFF) && (timeout != 0));
    
    return status;
}