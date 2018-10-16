#include "main.h"
#include "uart.h"

void UART_init(uint16_t baud_rate) {
    // configure UART
    PCON &= 0x3F; // clear SMOD0 and SMOD1
    PCON |= (SMOD1 << 7); // set or clear SMOD1

    SCON = UART_MODE_1 | RECEIVE_ENABLE;

    // initialize baud rate generator
    BDRCON = 0; // disable baud rate generator
    
    // set the baud rate reload
    BRL = (uint8_t)(256 - ((1 + (5 * SPD)) * (1 + (1 * SMOD1)) * OSC_FREQ) / (32UL * OSC_PER_INST * baud_rate));
    
    // enable baud rate generator for RxD and TxD
    BDRCON = (0x1C | (SPD << 1));

    // disable serial interrupt
    ES = 0;

    // initially not busy
    TI = 1;
}

uint8_t UART_transmit(uint8_t send_value) {
    uint8_t return_value;
    uint16_t timeout;

    timeout = 0;

    do {
        timeout++;
    } while ((TI == 0) && (timeout != 0));

    if (timeout != 0) {
        SBUF = send_value;
        return_value = send_value;
        TI = 0;
    }
    else {
        return_value = UART_TIMEOUT;
    }

    return return_value;
}

uint8_t UART_receive(void) {
    uint8_t rec_value;
    
    while (RI == 0);
    rec_value = SBUF;
    RI = 0;
    
    return rec_value;
}