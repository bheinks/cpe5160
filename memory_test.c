#include "main.h"
#include "memory_test.h"

static uint8_t code code_mem[]="Code Memory: String for Brett Heinkel";
static uint8_t xdata xdata_mem[50];

/***********************************************************************
DESC:  Returns the pointer to a string in code memory
INPUT: global string definition
RETURNS: pointer to the global string definition
CAUTION: 
************************************************************************/
uint8_t * code_memory_init(void)
{
   return code_mem;
}


/***********************************************************************
DESC:  Copies a string of characters from code memory to xram
INPUT: uses two global pointers defined above
RETURNS: pointer to the string in xdata memory
CAUTION: 
************************************************************************/
uint8_t * xdata_memory_init(void)
{
   uint8_t index, value;
   
   index=0;
   do
   {
       value=code_mem[index];
       xdata_mem[index]=value;
       index++;
   }while(value!=0);
   xdata_mem[0]='X';
   xdata_mem[1]='R';
   xdata_mem[2]='A';
   xdata_mem[3]='M';
   

   return xdata_mem;
}
