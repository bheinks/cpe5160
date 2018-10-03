#include "uart.h"
#include "AT89C51RC2.h"

void UART_Init() {
	BDRCON = 0; // disable baud rate generator
	ES = 0;  // disable serial interrupt
	
	// clear or set SMOD1 according to constant
	if (SMOD1 == 0) {
		PCON &= 0x7F;
	}
	else {
		PCON |=	0x80;
	}
	
	// clear SMOD0
	PCON &= 0xBF;
	
	// 
	SCON = 0x50;
	
	BRL = BRL_CALC;
	BDRCON = (0x1C | (SPD << 1));
	TI = 1;  //Make this last
}

uint8_t UART_Transmit(uint8_t send_value) {
	while (!TI);
	SBUF = send_value;
	TI = 0;
	
	return send_value;
}

uint8_t UART_Receive(void) {
	uint8_t received_value;
	
	while(RI == 0);
	received_value = SBUF;
	RI = 0;
	
	return received_value;
}