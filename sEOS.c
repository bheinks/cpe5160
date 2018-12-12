#include "sEOS.h"
#include "PORT.h"
#include "main.h"

void seos_init(uint8_t interval) {
    uint32_t increments;
    uint16_t reload_16;
    uint8_t reload_8H, reload_8L;
    
    T2CON = 0x04; // load timer 2 control register
    
    // number of timer increments required (max 65536)
    increments = ((uint32_t)interval * (OSC_FREQ/1000)) / (uint32_t)OSC_PER_INST;
    
    reload_16 = (uint16_t)(65536UL - increments); // 16 bit reload value
    
    // 8 bit reload values
    reload_8H = (uint8_t)(reload_16 / 256);
    reload_8L = (uint8_t)(reload_16 % 256);
    
    // load timer 2 high byte
    TH2 = reload_8H;
    RCAP2H = reload_8H;
    
    // load timer 2 low byte
    TL2 = reload_8L;
    RCAP2L = reload_8L;
    
    ET2 = 1; // enable timer 2 interrupt
    TR2 = 1; // start timer 2 interrupt
    
    EA = 1; // enable interrupts globally
}

void go_to_sleep(void) {
    PCON |= 0x01; // enter idle mode
}