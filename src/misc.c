#include <intrins.h>
#include "misc.h"

//本语句执行一次需5us的时间
void delay_5us(unsigned char t) 
{
  while(--t);
}

void delay_ms(unsigned char t) 
{    
  unsigned char j;   
  while(t--) {      
      for(j=0;j<200;j++);   //延时1ms   
  } 
}