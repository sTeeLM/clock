#ifndef __CLOCK_I2C_H__
#define __CLOCK_I2C_H__

#ifdef __OSCILLATO_6T__
#define I2C_DELAY_VALUE  40  
#define I2C_STOP_WAIT_VALUE 80 
#else
#define I2C_DELAY_VALUE  20  
#define I2C_STOP_WAIT_VALUE 40  
#endif


void I2C_Init();
bit I2C_Put(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char dat);  
bit I2C_Get(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char * dat);

bit I2C_Gets(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat);
bit I2C_Puts(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char Size, unsigned char *dat);

#endif