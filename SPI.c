/*#include "SPI.h"
#include <stdio.h>

uint8_t SPI_master_init(uint32_t clock_freq) {
    uint8_t divider, return_value;
    
    divider = (OSC_FREQ * 6) / (OSC_PER_INST * clock_freq);
    return_value = 0;
    
    // set SPCON depending on divider value
    if (divider < 2) {
        SPCON = 0x70;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 4) {
        SPCON = 0x71;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 8) {
        SPCON = 0x72;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 16) {
        SPCON = 0x73;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 32) {
        SPCON = 0xF0;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 64) {
        SPCON = 0xF1;// | (CPOL << 3) | (CPHA << 2);
    }
    else if (divider < 128) {
        SPCON = 0xF2;// | (CPOL << 3) | (CPHA << 2);
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
        printf("(SPI_transfer) TIMEOUT_ERROR\n");
    }
    else if ((status & 0x70) != 0) {
        error_flag = SPI_ERROR;
        *rec_value = 0xFF;
        printf("(SPI_transfer) SPI_ERROR\n");
    }
    else {
        error_flag = NO_ERROR;
        *rec_value = SPDAT;
        //printf("(SPI_transfer) NO_ERROR\n");
    }
    
    return error_flag;
}*/

#include "main.h"
#include "PORT.H"
#include "SPI.h"


/***********************************************************************
DESC:    Sets up the SPI to master mode with the clock as close
         to the input parameter as possible.
         clock=32-bit 
RETURNS: Error Flag
CAUTION: Sets the CPHA to 0 and CPOL to 0
         Disables SS and sets master mode 
************************************************************************/

uint8_t SPI_master_init(uint32_t clock_rate)
{
  uint8_t divider,return_val;
  return_val=0;
  divider=(uint8_t)(((OSC_FREQ/OSC_PER_INST)*6)/clock_rate);
  if(divider<=2)
  {
     SPCON=0x70;
  }
  else if((divider>2)&&(divider<=4))
  {
     SPCON=0x71;
  }
  else if((divider>4)&&(divider<=8))
  {
     SPCON=0x72;
  }
  else if((divider>8)&&(divider<=16))
  {
     SPCON=0x73;
  } 
  else if((divider>16)&&(divider<=32))
  {
     SPCON=0xF0;
  }
  else if((divider>32)&&(divider<=64))
  {
     SPCON=0xF1;
  }
  else if((divider>64)&&(divider<=128))
  {
     SPCON=0xF2;
  }
  else  // if the SPI clock rate is too slow, a divider cannot be found
  {
    return_val=CLOCK_RATE_ERROR;
  }
  return return_val;
}

/***********************************************************************
DESC:    Sends one byte using the SPI port and returns the received byte
          
RETURNS: SPI Error Flags | received byte
         or a timeout error
CAUTION: Waits for the SPI transfer to be complete
************************************************************************/


uint8_t SPI_transfer(uint8_t data_input, uint8_t * data_output)
{
   uint8_t test, timeout;
   timeout=0;
   SPDAT=data_input;
   do
   {
      test=SPSTA;
	  timeout++;
   }while(((test&0xF0)==0)&&(timeout!=0));
   if(timeout!=0)
   {
     if((test&0x70)==0)  // no errors
     {
         *data_output=SPDAT;
         timeout=NO_ERROR;
     }
     else
     {
         *data_output=0xff;
         timeout=SPI_ERROR;
     }
   }
   else
   {
     *data_output=0xff;
     timeout=TIMEOUT_ERROR;
   }
   return timeout;
}
