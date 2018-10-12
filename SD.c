#include "SD.h"
#include "SPI.h"
#include "STDIO.H"
#include "PORT.h"
#include "Long_Serial_In.h"

sbit green = P2^7;
sbit orange = P2^6;
sbit yellow = P2^5;
sbit red = P2^4;

static uint8_t  idata SD_Card_Type;

/*uint8_t send_command(uint8_t command, uint32_t argument)
{
    uint8_t rec_value, argument_LSB, argument_byte1, argument_byte2, argument_MSB, command_end, return_value, error_flag;
    
    // check if CMD value is valid (less than 64)
    if(command < 64){
        return_value = NO_ERROR;
    }
    else{
        return_value = ILLEGAL_COMMAND;
        return return_value;
    }
    
    // append start and transmission bits to the 6 bit command
    command |= 0x40;
    
    // set commandend based on necessary CRC7 and end bit
    if(command == 0x40){      // CMD0
        command_end = 0x95;
    }
    else if(command == 0x48){ // CMD8
        command_end = 0x87;
    }
    else{                    // all other commands require no checksum
        command_end = 0x01;
    }
    
    // split argument into 4 bytes
    argument_LSB = argument & 0xFF;
    argument_byte1 = (argument >> 8) & 0xFF;
    argument_byte2 = (argument >> 16) & 0xFF;
    argument_MSB = (argument >> 24) & 0xFF;
    
    // send 6 byte command while checking for errors
    error_flag = SPI_transfer(command, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }
    
    error_flag = SPI_transfer(argument_MSB, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_byte2, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_byte1, &rec_value);
    if(error_flag != NO_ERROR) {
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(argument_LSB, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }    
    
    error_flag = SPI_transfer(command_end, &rec_value);
    if(error_flag != NO_ERROR){
        return_value = SPI_ERROR;
        return return_value;
    }
    
    return return_value;
}*/

uint8_t SEND_COMMAND(uint8_t cmnd, uint32_t argum)
{
 uint8_t SPI_send, return_val, SPI_return, error_flag;

 return_val=NO_ERROR;
 if(cmnd<64)
 {
   SPI_send=cmnd | 0x40;
   error_flag=SPI_transfer(SPI_send,&SPI_return);
   if((error_flag)==NO_ERROR)
   {
     SPI_send=argum>>24;   // MSB
     error_flag=SPI_transfer(SPI_send,&SPI_return);
   }
   else
   {
     return_val=SPI_ERROR;
   }
   if((return_val==NO_ERROR)&&(error_flag==NO_ERROR))
   {
     argum=argum & 0x00ffffff;
     SPI_send=argum>>16;  // BYTE2
     error_flag=SPI_transfer(SPI_send,&SPI_return);
   }
   else
   {
     return_val=SPI_ERROR;
   }
   if((return_val==NO_ERROR)&&(error_flag==NO_ERROR))
   {
     argum=argum & 0x0000ffff;
     SPI_send=argum>>8;   // BYTE1
     error_flag=SPI_transfer(SPI_send,&SPI_return);
   }
   else
   {
     return_val=SPI_ERROR;
   }     
   if((return_val==NO_ERROR)&&(error_flag==NO_ERROR))
   {
     SPI_send=argum & 0x000000ff;  // LSB
     error_flag=SPI_transfer(SPI_send,&SPI_return);
   }
   else
   {
     return_val=SPI_ERROR;
   }
   if((return_val==NO_ERROR)&&(error_flag==NO_ERROR))
   {         
      if (cmnd == 0)
      {
         SPI_send=0x95;  // CRC7 and end bit for CMD0
      }
      else if (cmnd == 8)
      {
         SPI_send=0x87;   // CRC7 and end bit for CMD8
      }
      else
      {
         SPI_send=0x01;  // end bit only for other commands
      }
      error_flag=SPI_transfer(SPI_send,&SPI_return);
    }
 }
 else
 {
   return_val=ILLEGAL_COMMAND;
 }
 return return_val;  
}

/*uint8_t receive_response(uint8_t num_bytes, uint8_t *byte_array)
{
    uint8_t SPI_val, count, error_flag, response;
    response = NO_ERROR;
    count = 0;

    // Keep transmitting until a response is received or timeout occurs
	do
    {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        printf("SPI_val: %2.2bX\n", SPI_val);
        count++;
    }while(((SPI_val&0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
	 
	// Error handling
    if (error_flag != NO_ERROR)
    {
        response = SPI_ERROR;
        printf("SPI_ERROR\n");
    }
	else if (count == 0)
    {
		response = TIMEOUT_ERROR;
        printf("TIMEOUT_ERROR\n");
    }
	else if ((SPI_val&0xFE) != 0x00)
    {
        *byte_array = SPI_val;
        response = COMM_ERROR;
        printf("COMM_ERROR\n");
    }
       
	// No errors found
	else
	{
        *byte_array = SPI_val;
        if(num_bytes > 1) // If the response is greater than one byte
        {
            for(count = 1; count < num_bytes; count++)
            {
                error_flag = SPI_transfer(0xFF, &SPI_val);
                *(byte_array + count) = SPI_val;
            }
        }
        else
        {
          response = COMM_ERROR;
          printf("COMM_ERROR2\n");
        }
    }
    error_flag = SPI_transfer(0xFF, &SPI_val);  // End with sending one last 0xFF out of the SPI port
			
    return response;
}*/

uint8_t receive_response(uint8_t num_bytes, uint8_t * valout)
{
   uint8_t index,return_val,error_flag, SPI_return;

   return_val=NO_ERROR;
   do
   {
      error_flag=SPI_transfer(0xFF,&SPI_return);
      index++;
   }while(((SPI_return&0x80)==0x80)&&(index!=0)&&(error_flag==NO_ERROR));
   if(error_flag!=NO_ERROR)
   {
      return_val=SPI_ERROR;
   }
   else if(index==0)
   {
      return_val=TIMEOUT_ERROR;
   }
   else
   {
     *valout=SPI_return;
     if((SPI_return==0x00)||(SPI_return==0x01))
     {
       if(num_bytes>1)
       {
         for(index=1;index<num_bytes;index++)
         {
            error_flag=SPI_transfer(0xFF,&SPI_return);
            *(valout+index)=SPI_return;
         }
       }
     }
     else
     {
        return_val=COMM_ERROR;
     }
   }
   error_flag=SPI_transfer(0xFF,&SPI_return);  // send 8 more clock cycles to complete read
   return return_val;
}


/*uint8_t SD_card_init(void) {
    uint8_t index, receive_array[8], error_flag, timeout, return_value, i;

    timeout = 1; //initialize timeout variable
    
    printf("Initializing SD card...\n");
    
    // 74+ clock pulses on SCK with nCS high
    nCS0 = 1;
    for(index = 0; index < 10; ++index) {
        error_flag = SPI_transfer(0xFF, &return_value);
    }
    
    /************
    *
    *  command 0
    *
    *************/
    
/*    green = 0;
    
    
    nCS0 = 0;
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("SCK init error\n");
        return SD_INIT_ERROR;
    }
    
    //send CMD0 to SD card
    printf("CMD0 sent...\n");
    error_flag = send_command(CMD0, 0);
    
    //check for error
    if(error_flag != NO_ERROR){
        printf("CMD0 send error\n");
        return SD_INIT_ERROR;
    }
    
    //receive response from SD card
    error_flag=receive_response(1, receive_array);
    nCS0 = 1;
    
    printf("R1 Response expected\n");
    if(error_flag != NO_ERROR){
        printf("CMD0 receive error\n");
        return SD_INIT_ERROR;
    }
    else if(receive_array[0] != 0x01){
        printf("CMD0 response incorrect");
        printf("Response received: 0x");
        printf("%2.2bX", receive_array[0]);
        printf("...\n");
        return SD_INIT_ERROR;
    }
    
    printf("Response received: 0x");
    printf("%2.2bX", receive_array[0]);
    printf("...\n");
    
    /************
    *
    *  command 8
    *
    *************/
    
/*    nCS0 = 0;
    // Send CMD8 to SD card
    error_flag = send_command(CMD8, 0x000001AA);
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD8 send error\n");
        return SD_INIT_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD8 receive error\n");
        return SD_INIT_ERROR;
    }
    // Print results
    printf("R7 Response expected\n");
    if(receive_array[0] == 0x05){
        printf("Version 1 SD Card is Not Supported\n");
        return SD_INIT_ERROR;
    }
    else{
        printf("Version 2 Card Detected\n");
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2bX", receive_array[i]);
        }
        printf("...\n");
    }
    
    // Check for correct voltage
    if(receive_array[4] != 0x01){
        printf("CMD8 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    
    // Check for matching check byte
    if(receive_array[5] != 0xAA){
        printf("CMD8 check byte mismatch\n");
        return SD_INIT_ERROR;
    }
    
    /************
    *
    *  CMD58
    *
    *************/
        
/*    nCS0 = 0;
    // Send CMD58 to SD card
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD58 send error\n");
        return SD_INIT_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD58 receive error\n");
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("R3 Response expected\n");
    if((receive_array[2]&0xFC) != 0xFC){
        printf("CMD58 incorrect voltage error\n");
        return SD_INIT_ERROR;
    }
    else{
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2bX", receive_array[i]);
        }
        printf("...\n");
    }
    
    /************
    *
    *  ACMD41
    *
    *************/
        
/*    nCS0 = 0;
    // Send CMD55 to SD card
    error_flag = send_command(CMD55, 0);
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD55 send error\n");
        return SD_INIT_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(1, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD55 receive error\n");
        return SD_INIT_ERROR;
    }
    
    // Sending command ACMD41 until the R1 response is 0 or a timeout occurs
    while(receive_array[0] != 0){
        
        // Send ACMD41 to SD card
        error_flag = send_command(ACMD41, 0x40000000);
    
        // Check for error
        if(error_flag != NO_ERROR){
            printf("ACMD41 send error\n");
            return SD_INIT_ERROR;
        }
    
        // Receive response from SD card
        error_flag=receive_response(1, receive_array);
    
        // Check for error
        if(error_flag != NO_ERROR){
            printf("ACMD41 receive error\n");
            return SD_INIT_ERROR;
        }
        
        // Incriment timeout and check to see if it has reloaded
        timeout++;
        if(timeout == 0){
            printf("ACMD41 timeout error\n");
            return SD_INIT_ERROR;
        }
        
        nCS0 = 1;
        
    }
    
    // Print results
    printf("Response received: 0x");
    printf("%2.2bX", receive_array[0]);
    printf("...\n");

    /************
    *
    *  CMD58 Again
    *
    *************/
        
/*    nCS0 = 0;
    // Send CMD58 to SD card
    error_flag = send_command(CMD58, 0);
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD58 send error\n");
        return SD_INIT_ERROR;
    }
    
    // Receive response from SD card
    error_flag=receive_response(5, receive_array);
    nCS0 = 1;
    
    // Check for error
    if(error_flag != NO_ERROR){
        printf("CMD58 receive error\n");
        return SD_INIT_ERROR;
    }
    
    // Print results
    printf("R3 Response expected\n");
    if((receive_array[1]&0x80) != 0x80){
        printf("CMD58 card not in active state error\n");
        return SD_INIT_ERROR;
    }
    else if(receive_array[1]&0xC0 == 0xC0){
        printf("High capacity card accepted");
        printf("Response received: 0x");
        for(i=0;i<5;i++){
            printf("%2.2bX", receive_array[i]);
        }
        printf("...\n");
    }
    else{
        printf("High capacity card not detected error");
        return SD_INIT_ERROR;
    }
        
    printf("Initialization of SD card complete...\n");
    
    return NO_ERROR;
}*/

uint8_t SD_card_init(void)
{
   uint8_t i,error_status,error_flag,valsout[8],SPI_return;
   uint32_t argument;
   uint16_t timeout;
   error_status=NO_ERROR;
   SD_Card_Type=unknown;
   nCS0=1;
   printf("SD Card Initialization ... \n\r");
   for(i=0;i<10;i++)
   {
       error_flag=SPI_transfer(0xFF,&SPI_return);
   }
   GREENLED=0;
   printf("CMD0 sent ... ");   
   nCS0=0;  // CS card CS low
   error_flag=SEND_COMMAND(CMD0,0);
   if(error_flag==NO_ERROR)
   {
     error_flag=receive_response(1,valsout);
	 nCS0=1;
     GREENLED=1;
     printf("Response = %2.2bX\n\r",valsout[0]);
   }
   else
   {
     nCS0=1;
	 GREENLED=1;
	 REDLED=0;  // indicates error has occured.
   }
   if(error_flag!=NO_ERROR)
   {
     error_status=error_flag;
   }
   if(error_status==NO_ERROR)
   {
     GREENLED=0;
     printf("CMD8 sent ... ");
     nCS0=0;
     error_flag=SEND_COMMAND(CMD8,0x000001AA);
	 if(error_flag==NO_ERROR)
     {
        error_flag=receive_response(5,valsout);
		nCS0=1;
		GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
		        printf("%2.2bX ",valsout[i]);
		}
        putchar(CR);
        putchar(LF);
		if(valsout[4]!=0xAA)
		{
		   error_flag=response_error;
		}
     }
     else
     {
        nCS0=1;
        GREENLED=1;
	    REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=NO_ERROR)
	 {
        if(error_flag==illegal_cmnd)
        {
           error_status=NO_ERROR;
		   SD_Card_Type=Standard_Capacity;
		   printf("Version 1 SD Card detected.\n\r");
		   printf("Standard Capacity Card detected.\n\r");
        }
		else
		{
		   error_status=error_flag;
		}
	 }
	 else
	 {
		SD_Card_Type=Ver2;
		printf("Version 2 SD Card detected.\n\r");
	 }
   }
   if(error_status==NO_ERROR)
   {
     GREENLED=0;
     printf("CMD58 sent ... ");
     nCS0=0;
     error_flag=SEND_COMMAND(CMD58,0);
	 if(error_flag==NO_ERROR)
     {
        error_flag=receive_response(5,valsout);
		nCS0=1;
		GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
		     printf("%2.2bX ",valsout[i]);
		}
        putchar(CR);
        putchar(LF);
		if((valsout[2]&0xFC)!=0xFC)
		{
		   error_flag=voltage_error;
		}
     }
     else
     {
        nCS0=1;
        GREENLED=1;
	    REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=NO_ERROR)
	 {		
	    error_status=error_flag;
	 }
   }
   if(error_status==NO_ERROR)
   {
     if(SD_Card_Type==Ver2)
	 {
	    argument=0x40000000;
	 }
	 else
	 {
	    argument=0;
	 }
	 timeout=0;
     GREENLED=0;
     printf("ACMD41 sent ... ");
     nCS0=0;
	 do
	 {

	    error_flag=SEND_COMMAND(CMD55,0);
		if(error_flag==NO_ERROR)  error_flag=receive_response(1,valsout);

	    if((valsout[0]==0x01)||(valsout[0]==0x00))
		   error_flag=SEND_COMMAND(ACMD41,argument);
	    if(error_flag==NO_ERROR) receive_response(1,valsout);
		timeout++;
		if(timeout==0) error_flag=timeout_error;
	   }while(((valsout[0]&0x01)==0x01)&&(error_flag==NO_ERROR));
	 if(error_flag==NO_ERROR)
     {
		nCS0=1;
        GREENLED=1;
	    printf("Response = %2.2bX\n\r",valsout[0]);
     }
     else
     {
        nCS0=1;
        GREENLED=1;
	    REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=NO_ERROR)
	 {		
	    error_status=error_flag;
	 }
   }
   if((error_status==NO_ERROR)&&(SD_Card_Type==Ver2))
   {
     GREENLED=0;
     printf("CMD58 sent ... ");
     nCS0=0;
     error_flag=SEND_COMMAND(CMD58,0);
	 if(error_flag==NO_ERROR)
     {
        error_flag=receive_response(5,valsout);
		nCS0=1;
        GREENLED=1;
	    printf("Response = ");
        for(i=0;i<5;i++)
		{   
           printf("%2.2bX ",valsout[i]);
		}
        putchar(CR);
        putchar(LF);
		if((valsout[1]&0x80)!=0x80)
		{
		   error_flag=card_inactive;
		}
		else
		{
		   if((valsout[1]&0xC0)==0xC0)
		   {
		      SD_Card_Type=High_Capacity;
		      printf("High Capacity Card Detected\n\r");
		   }
		   else
		   {
		      SD_Card_Type=Standard_Capacity;
			  printf("Standard Capacity Card Detected\n\r");
			  GREENLED=0;
              printf("CMD16 sent ... ");
              nCS0=0;
              error_flag=SEND_COMMAND(CMD16,512);
	          if(error_flag==NO_ERROR)
              {
                  error_flag=receive_response(1,valsout);
		          nCS0=1;
                  GREENLED=1;
	              printf("Response = %2.2bX \n\r",valsout[0]);
                  printf("Block size set to 512 bytes\n\r");
		      }
			}
		 }
     }
     else
     {
        nCS0=1;
        GREENLED=1;
	    REDLED=0;  // indicates error has occured.
     }
	 if(error_flag!=NO_ERROR)
	 {		
	    error_status=error_flag;
		print_error(error_status);
	 }
   }
 if(error_status!=NO_ERROR)
 {		
	print_error(error_status);
 }
return error_status;
}

uint8_t read_block(uint16_t num_bytes, uint8_t * byte_array) {
    uint8_t SPI_val, count, error_flag, response;
    
    response = NO_ERROR;
    count = 0;
    
    // keep transmitting until a response is received or timeout occurs
	do {
        error_flag = SPI_transfer(0xFF, &SPI_val);
        count++;
    } while (((SPI_val & 0x80) == 0x80) && (error_flag == NO_ERROR) && (count != 0));
    
    // error handling
    if (error_flag != NO_ERROR) {
        response = SPI_ERROR;
    }
	else if (count == 0) {
		response = TIMEOUT_ERROR;
    }
	else if ((SPI_val & 0xFE) != 0x00) {
        *byte_array = SPI_val;
        response = COMM_ERROR;
    }
    else {
        count = 0;
        
        // wait for data token
        do {
            error_flag = SPI_transfer(0xFF, &SPI_val);
            count++;
        } while ((SPI_val == 0xFF) && (error_flag == NO_ERROR) && (count != 0));
        
        if (error_flag != NO_ERROR) {
            response = SPI_ERROR;
        }
        else if (count == 0) {
            response = TIMEOUT_ERROR;
        }
        else if (SPI_val == 0xFE) {
            for (count = 0; count < num_bytes; ++count) {
                error_flag = SPI_transfer(0xFF, &SPI_val);
                *(byte_array + count) = SPI_val;
            }
            
            // discard CRC
            error_flag = SPI_transfer(0xFF, &SPI_val);
            error_flag = SPI_transfer(0xFF, &SPI_val);
        }
        else {
            response = DATA_ERROR;
        }
    }
    
    // return to standby state
    error_flag = SPI_transfer(0xFF, &SPI_val);
    
    return response;
}

void print_error(uint8_t error)
{
   if(error==timeout_error) printf("Timeout Error");
   else if(error==illegal_cmnd) printf("Illegal Command\n\r");
   else if(error==response_error) printf("Response Error");
   else if(error==data_error) printf("Data Token Error");
   else if(error==voltage_error) printf("Incompatible Voltage");
   else if(error==card_inactive) printf("Card is Inactive");
   else if(error==SPI_error) printf("SPI or Timeout Error");
   else printf("Unknown Error");
}