#include <intrins.h>
#include "misc.h"

//�����ִ��һ����5us��ʱ��
void delay_5us(unsigned char t) 
{
  while(--t);
}

void delay_ms(unsigned char t) 
{    
  unsigned char j;   
  while(t--) {      
      for(j=0;j<200;j++);   //��ʱ1ms   
  } 
}