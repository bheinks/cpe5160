#include "I2C.h"
#include "PORT.h"

#define I2C_FREQ 25000UL
#define I2C_RELOAD (65536-((OSC_FREQ)/(OSC_PER_INST*I2C_FREQ*2)))
#define I2C_RELOAD_H (I2C_RELOAD/256)
#define I2C_RELOAD_L (I2C_RELOAD%256)

#define CONTINUE 1
#define STOP 2

void I2C_clock_start(void);
void I2C_clock_delay(uint8_t control);

uint8_t I2C_write(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array) {
    uint8_t send_value, i, num_bits, send_bit, sent_bit;
    
    SDA = 1;
    SCL = 1;
    
    if ((SCL == 1) && (SDA == 1)) {
        I2C_clock_start();
        
        // send device address with LSB 0 for write
        send_value = device_addr << 1;
        SDA = 0;
        num_bytes++;
        
        // for every byte of data
        for (i = 0; i < num_bytes; ++i) {
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
            
            I2C_clock_delay(CONTINUE);
            
            SCL = 0;
            SDA = 1;
            
            send_value = *(byte_array);
            byte_array++;
            
            I2C_clock_delay(CONTINUE);
            
            SCL = 1;
            while (SCL != 1);
            
            sent_bit = SDA;
            if (sent_bit != 0) {
                return NACK_ERROR;
            }
        }
        
        // stop condition
        I2C_clock_delay(CONTINUE);
        
        SCL = 0;
        SDA = 0;
        
        I2C_clock_delay(CONTINUE);
        
        SCL = 1;
        while (SCL != 1);
        
        I2C_clock_delay(STOP);
        SDA = 1;
    }
    else {
        return BUS_BUSY_ERROR;
    }
    
    return NO_ERROR;
}

uint8_t I2C_read(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * byte_array) {
    uint8_t send_value, i, num_bits, send_bit, sent_bit;
    
    SDA = 1;
    SCL = 1;
    
    if ((SCL == 1) && (SDA == 1)) {
        I2C_clock_start();
        

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