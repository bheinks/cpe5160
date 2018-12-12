#include "traffic_light.h"
#include "PORT.h"

// Function to set lights based on input numbers
void set_lights(bit green, bit amber, bit yellow, bit red, bit blue) {
    GREENLED = ~green;
    AMBERLED = ~amber;
    YELLOWLED = ~yellow;
    REDLED = ~red;
    BLUELED = ~blue;
}

// Function to control traffic light system state
void traffic_light_isr(void) interrupt TIMER_2_OVERFLOW {
    TF2 = 0; // clear interrupt flag
    TIMER++; // increment timer
    FLASH_TIMER++; // increment flash timer
    
    /*
    --Notes on switch state--
    TIMER reset before switching to any state that requires the timer
    FLASH_TIMER reset before any state that requires the flash timer
    No waiting in any state to avoid delaying past system tick
    */
    
    switch (SYSTEM_STATE) {
        case MAIN_GREEN:
            set_lights(1, 0, 0, 0, 0);
        
            if (!SW1) { // Represents car in side street near light
                SYSTEM_STATE = MAIN_YELLOW_SIDE;
                TIMER = 0;
            }
            else if (!SW2) {  // Represents cross street button
                SYSTEM_STATE = MAIN_YELLOW_CROSS;
                TIMER = 0;
                
            }
        
            break;
        case MAIN_YELLOW_CROSS:
            set_lights(0, 0, 1, 0, 0);
        
            if (TIMER >= 1000) { // Transiition to red main light after 2 seconds
                SYSTEM_STATE = MAIN_RED_CROSS;
                TIMER = 0;
            }
        
            break;
        case MAIN_YELLOW_SIDE:
            set_lights(0, 0, 1, 0, 0);
        
            if (TIMER >= 1000) { // Transition to red main light after 2 seconds
                SYSTEM_STATE = MAIN_RED_SIDE;
                TIMER = 0;
            }
        
            break;
        case MAIN_RED_CROSS:
            set_lights(0, 0, 0, 1, 0);
        
            if (TIMER >= 500) { // Transition to walk light after 1 second
                SYSTEM_STATE = WALK; 
                TIMER = 0;
            }
        
            break;
        case MAIN_RED_SIDE:
            set_lights(0, 0, 0, 1, 0);
        
            if (TIMER >= 500) { // Transition to side green state after 1 second
                SYSTEM_STATE = SIDE_GREEN;
                TIMER = 0;
            }
        
            break;
        case SIDE_BUTTON_RELEASE:
            set_lights(1, 0, 0, 1, 0);
            
            if (TIMER >= 2500){ // Transition to side yellow state after 5 seconds of waiting
                SYSTEM_STATE = SIDE_YELLOW;
                TIMER = 0;
            }
            
            break;
        case SIDE_RED:
            set_lights(0, 0, 0, 1, 0);
        
            if (TIMER >= 500) { // Transition to main light green after 1 second
                SYSTEM_STATE = MAIN_GREEN;
                TIMER = 0;
            }
            
            break;
        case SIDE_YELLOW:
            set_lights(0, 0, 1, 1, 0);
        
            if (TIMER >= 1000) { // Transition to side light red after 2 seconds
                SYSTEM_STATE = SIDE_RED;
                TIMER = 0;
            }
            
            break;
        case SIDE_GREEN:
            set_lights(1, 0, 0, 1, 0);
        
            if (TIMER >= 15000){ // If button is help for full 30 seconds transition to yellow state
                SYSTEM_STATE = SIDE_YELLOW;
                TIMER = 0;
            }
            else if (SW1){ // If button is released transition to waiting state
                SYSTEM_STATE = SIDE_BUTTON_RELEASE;
                TIMER = 0;
            }
            
            break;
        case WALK:
            set_lights(0, 0, 0, 1, 1);
        
            if (TIMER >= 2500) { // Transition to don't walk flasher after 5 seconds
                SYSTEM_STATE = NO_WALK_ON;
                TIMER = 0;
                FLASH_TIMER = 0;
            }
        
            break;
        case NO_WALK_ON:
            set_lights(0, 1, 0, 1, 0);
        
            if (TIMER >= 2500) { // Transition to main light green after 5 seconds of flashing
                SYSTEM_STATE = MAIN_GREEN;
            }
            if (FLASH_TIMER >= 250) { // Blink every half second
                SYSTEM_STATE = NO_WALK_OFF;
                FLASH_TIMER = 0;
            }
        
            break;
        case NO_WALK_OFF:
            set_lights(0, 0, 0, 1, 0);
        
            if (TIMER >= 2500) { // Transition to main light green after 5 seconds of flashing
                SYSTEM_STATE = MAIN_GREEN;
            }
            if (FLASH_TIMER >= 250) { // Blink every half second
                SYSTEM_STATE = NO_WALK_ON;
                FLASH_TIMER = 0;
            }
        
            break;
    }
}