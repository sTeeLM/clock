#include <STC89C5xRC.H>
#include <intrins.h>
#include "i2c.h"

void I2C_Delay()
{
  unsigned char c;
  for(c  = 0 ; c < I2C_DELAY_VALUE; c++)
    _nop_(); 
}

sbit I2C_SCL = P1 ^ 7;  
sbit I2C_SDA = P1 ^ 6; 

void I2C_Init()
{
 I2C_SCL = 1;
 I2C_Delay();
 I2C_SDA = 1;
 I2C_Delay();
}


void I2C_Start()
{
 //EA=0;

 I2C_SCL = 1;
 I2C_Delay();
 I2C_SDA = 1;
 I2C_Delay();
 I2C_SDA = 0;
 I2C_Delay();
 I2C_SCL = 0;
 I2C_Delay();
 I2C_Delay();
 I2C_Delay();
}

void I2C_Stop()
{
 unsigned int t = I2C_STOP_WAIT_VALUE;

 I2C_SDA = 0;
 I2C_Delay();

 I2C_SCL = 1;
 I2C_Delay();
 I2C_SDA = 1;
 I2C_Delay();
  
 //EA=1;
 
 while ( --t != 0 );
}



void I2C_Write(unsigned char dat)
{

 unsigned char t ;
 for(t=0;t<8;t++)
 {

  I2C_SDA = (bit)(dat & 0x80);
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  I2C_SCL = 0;  
  I2C_Delay();
  dat <<= 1;
 }


}

unsigned char I2C_Read()
{
 unsigned char dat=0;
 unsigned char t ;
 bit temp;
 I2C_Delay();
 I2C_Delay();
 I2C_SDA = 1;

 I2C_Delay();
 
 for(t=0;t<8;t++)
 {
  I2C_SCL = 0;
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  temp = I2C_SDA;
  dat <<=1;
  if (temp==1) dat |= 0x01;   
 }
 
  I2C_SCL = 0;
  I2C_Delay();

 return dat;
}



bit I2C_GetAck()
{
 bit ack;
 unsigned char Error_time=255;

 I2C_Delay();  
 I2C_SDA = 1;
 I2C_Delay();
 I2C_SCL = 1;
 I2C_Delay();
 do
 {  
  ack = I2C_SDA;
    Error_time--;
   if(Error_time==0)
   {
     I2C_SCL = 0;
   I2C_Delay();
   return 1;
   }
 }while(ack);


 I2C_SCL = 0;
 I2C_Delay();
 I2C_Delay();
 I2C_Delay();
 return 0;
}

void I2C_PutAck(bit ack)
{

 I2C_SDA = ack;
 I2C_Delay();
 I2C_SCL = 1;
 I2C_Delay(); 

 I2C_SCL = 0;
 I2C_Delay();
 I2C_Delay();
 I2C_Delay();
 I2C_Delay();

}
/*
bit I2C_Put(unsigned char SlaveAddr, unsigned char SubAddr, char dat)
{
  SlaveAddr &= 0xFE;
  I2C_Start();
  I2C_Write(SlaveAddr);
  if ( I2C_GetAck() )
  {
    I2C_Stop();
    return 1;
  }

  I2C_Write(SubAddr);
  if ( I2C_GetAck() )
  {
    I2C_Stop();
    return 1;
  }
  I2C_Write(dat);
  if ( I2C_GetAck() )
  {
   I2C_Stop();
   return 1;
  } 
  I2C_Stop();
  return 0;  
}

bit I2C_Get(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char *dat)
{
 SlaveAddr &= 0xFE; 

 I2C_Start();

 I2C_Write(SlaveAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 I2C_Write(SubAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 I2C_Start();

 SlaveAddr |= 0x01;
 I2C_Write(SlaveAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

  *dat = I2C_Read();
  I2C_PutAck(1);

  I2C_Stop();
  return 0;
}

*/
bit I2C_Puts(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat)
{

 if ( Size == 0 ) return 0;

 SlaveAddr &= 0xFE;

 I2C_Start();

 I2C_Write(SlaveAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 I2C_Write(SubAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 do
 {
  I2C_Write(*dat++);
  if ( I2C_GetAck() )
  {
   I2C_Stop();
   return 1;
  }
 } while ( --Size != 0 );

 I2C_Stop();
 return 0;
}

bit I2C_Put(unsigned char SlaveAddr, unsigned char SubAddr, char dat)
{
 return I2C_Puts(SlaveAddr,SubAddr,1,&dat);
}


bit I2C_Gets(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat)
{
 if ( Size == 0 ) return 0;

 SlaveAddr &= 0xFE; 

 I2C_Start();

 I2C_Write(SlaveAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 I2C_Write(SubAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 I2C_Start();

 SlaveAddr |= 0x01;
 I2C_Write(SlaveAddr);
 if ( I2C_GetAck() )
 {
  I2C_Stop();
  return 1;
 }

 for (;;)
 {
  *dat++ = I2C_Read();
  if ( --Size == 0 )
  {
   I2C_PutAck(1);
   break;
  }
  I2C_PutAck(0);
 }

 I2C_Stop();
 return 0;
}

bit I2C_Get(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char *dat)
{
 return I2C_Gets(SlaveAddr,SubAddr,1,dat);
}