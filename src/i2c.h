#ifndef __CLOCK_I2C_H__
#define __CLOCK_I2C_H__

void I2C_Delay();
void I2C_Start();
void I2C_Stop();
void I2C_Write(unsigned char dat);
unsigned char I2C_Read();
bit I2C_GetAck();
void I2C_PutAck(bit ack);

bit I2C_Put(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char dat);  
bit I2C_Get(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char * dat);

bit I2C_Gets(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat);
bit I2C_Puts(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat);

#endif
