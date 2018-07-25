#include <STC89C5xRC.H>
#include <intrins.h>
#include "i2c.h"
#include "debug.h"
#include "misc.h"

sbit I2C_SCL = P1 ^ 7;  
sbit I2C_SDA = P1 ^ 6; 


void I2C_Delay()
{
  _nop_();
  _nop_();
  _nop_();
  _nop_();
}

void I2C_Init()
{
 I2C_SCL = 1;
 I2C_Delay();
 I2C_SDA = 1;
 I2C_Delay();
}


void I2C_Start()
{
  I2C_SDA = 1;
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  I2C_SDA = 0;
  I2C_Delay();
  I2C_SCL = 0;
  I2C_Delay();
}

void I2C_Stop()
{
  I2C_SDA = 0;
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  I2C_SDA = 1;
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
  I2C_Delay();
}



void I2C_Write(unsigned char dat)
{

  unsigned char t = 8;
  do
  {
     I2C_SDA = (bit)(dat & 0x80);
     dat <<= 1;
     I2C_SCL = 1;
     I2C_Delay();
     I2C_SCL = 0;
     I2C_Delay();
  } while ( --t != 0 );
}

unsigned char I2C_Read()
{
  char dat = 0;
  unsigned char t = 8;
  I2C_SDA = 1; //在读取数据之前，要把SDA拉高
  do
  {
     I2C_SCL = 1;
     I2C_Delay();
     dat <<= 1;
     if ( I2C_SDA ) dat |= 0x01;
     I2C_SCL = 0;
     I2C_Delay();
  } while ( --t != 0 );
  return dat;
}



bit I2C_GetAck()
{
  bit ack;
  I2C_SDA = 1;
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  ack = I2C_SDA;
  I2C_SCL = 0;
  I2C_Delay();
  return ack;
}

void I2C_PutAck(bit ack)
{
  I2C_SDA = ack;
  I2C_Delay();
  I2C_SCL = 1;
  I2C_Delay();
  I2C_SCL = 0;
  I2C_Delay();
}

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
