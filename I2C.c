#include "I2C.h"
#include "PORT.h"

#define I2C_FREQ 25000UL
#define I2C_RELOAD (65536-((OSC_FREQ)/(OSC_PER_INST*I2C_FREQ*2)))
#define I2C_RELOAD_H (I2C_RELOAD/256)
#define I2C_RELOAD_L (I2C_RELOAD%256)

#define CONTINUE 1
#define STOP 2

uint8_t I2C_send_byte(uint8_t send_value);
uint8_t I2C_acknowledge(void);
uint8_t I2C_receive_byte(void);
void I2C_clock_start(void);
void I2C_clock_delay(uint8_t control);

uint8_t I2C_send_byte(uint8_t send_value) {
    uint8_t num_bits, send_bit, sent_bit;
    
    // send bits starting with MSB
    for (num_bits = 7; num_bits >= 0; --num_bits) {
        I2C_clock_delay(CONTINUE);
        SCL = 0;
        
        send_bit = send_value >> num_bits;
        SDA = (bit)send_bit;
        
        I2C_clock_delay(CONTINUE);
        
        SCL = 1;
        while (SCL != 1);
        
        sent_bit = SDA;
        if (sent_bit != send_bit) {
            return BUS_BUSY_ERROR;
        }
    }
    
    return NO_ERROR;
}

uint8_t I2C_acknowledge(void) {
    uint8_t sent_bit;
    
    I2C_clock_delay(CONTINUE);
    
    SCL = 0;
    SDA = 1;
    
    I2C_clock_delay(CONTINUE);
    
    SCL = 1;
    while (SCL != 1);
    
    sent_bit = SDA;
    if (sent_bit != 0) {
        return NACK_ERROR;
    }
    
    return NO_ERROR;
}

uint8_t I2C_write(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array) {
    uint8_t i;
    
    // start condition begin
    SDA = 1;
    SCL = 1;
    
    if ((SCL == 1) && (SDA == 1)) {
        I2C_clock_start();

        SDA = 0;
        // start condition end
        
        // send device address with LSB 0 for r/w
        if (I2C_send_byte(device_addr << 1) != NO_ERROR) {
            return BUS_BUSY_ERROR;
        }
        
        if (I2C_acknowledge() != NO_ERROR) {
            return NACK_ERROR;
        }
        
        // send internal address
        if (int_addr_sz > 0) {
            for (i = 0; i < int_addr_sz; ++i) {
                // starting from LSB, send one byte at a time via truncation and bit shifting
                I2C_send_byte((uint8_t) (int_addr >> (8*i)));
            }
            
            if (I2C_acknowledge() != NO_ERROR) {
                return NACK_ERROR;
            }
        }
        
        // send data
        for (i = 0; i < num_bytes; ++i) {
            if (I2C_send_byte(byte_array[i]) != NO_ERROR) {
                return BUS_BUSY_ERROR;
            }
            
            if (I2C_acknowledge() != NO_ERROR) {
                return NACK_ERROR;
            }
        }
        
        // stop condition begin
        I2C_clock_delay(CONTINUE);
        
        SCL = 0;
        SDA = 0;
        
        I2C_clock_delay(CONTINUE);
        
        SCL = 1;
        while (SCL != 1);
        
        I2C_clock_delay(STOP);
        SDA = 1;
        // stop condition end
    }
    else {
        return BUS_BUSY_ERROR;
    }
    
    return NO_ERROR;
}

uint8_t I2C_receive_byte(void) {
    uint8_t num_bits, receive_value, sent_bit;
    receive_value = 0;
    
    for (num_bits = 7; num_bits >= 0; ++num_bits) {
        I2C_clock_delay(CONTINUE);
        
        SCL = 0;
        SDA = 1;
        
        I2C_clock_delay(CONTINUE);        
        
        SCL = 1;
        while (SCL != 1);
        
        receive_value <<= 1;
        sent_bit = SDA;
        receive_value |= sent_bit;
    }
    
    return receive_value;
}

uint8_t I2C_read(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array) {
    uint8_t return_value, i, send_bit;
    
    // if an internal address is passed, send it via I2C_write
    if (int_addr_sz > 0) {
        return_value = I2C_write(device_addr, int_addr, int_addr_sz, 0, 0);
        
        if (return_value != NO_ERROR) {
            return return_value;
        }
    }
    
    // start condition begin
    SDA = 1;
    SCL = 1;
    
    if ((SCL == 1) && (SDA == 1)) {
        I2C_clock_start();

        SDA = 0;
        // start condition end

        // send device address with LSB 1 for r/w
        if (I2C_send_byte((device_addr << 1) | 0x01) != NO_ERROR) {
            return BUS_BUSY_ERROR;
        }
        
        if (I2C_acknowledge() != NO_ERROR) {
            return NACK_ERROR;
        }
        
        for (i = 0; i < num_bytes; ++i) {
            *(byte_array + i) = I2C_receive_byte();
            
            if ((num_bytes - i) == 1) {
                send_bit = 1;
            }
            else {
                send_bit = 0;
            }
            
            I2C_clock_delay(CONTINUE);
            
            SCL = 0;
            SDA = send_bit;
            
            I2C_clock_delay(CONTINUE);
            
            SCL = 1;
            while(SCL != 1);
        }

        // stop condition begin
        I2C_clock_delay(CONTINUE);
        
        SCL = 0;
        SDA = 0;
        
        I2C_clock_delay(CONTINUE);
        
        SCL = 1;
        while(SCL != 1);
        
        I2C_clock_delay(STOP);
        SDA = 1;
        // stop condition end
    }
    else {
        return BUS_BUSY_ERROR;
    }
        
    return NO_ERROR;
}
        

void I2C_clock_start(void) {
    TMOD &= 0xF0; // clear all T0 bits (T1 left unchanged)
    TMOD |= 0x01; // set required T0 bits (T1 left unchanged)

    ET0 = 0; // no interrupts
    
    TH0 = I2C_RELOAD_H;
    TL0 = I2C_RELOAD_L;
    
    TF0 = 0; // clear overflow flag
    TR0 = 1; // start timer 0
}

void I2C_clock_delay(uint8_t control) {
    if (TR0 == 1) {
        while (TF0 == 0);
    }
    
    TR0 = 0;
    
    if (control == CONTINUE) {
        TH0 = I2C_RELOAD_H;
        TL0 = I2C_RELOAD_L;
        
        TF0 = 0; // clear overflow flag
        TR0 = 1; // start timer
    }
}