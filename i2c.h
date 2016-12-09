#ifndef __CLOCK_I2C_H__
#define __CLOCK_I2C_H__

#include <REGX52.H>

#define I2C_DELAY_VALUE  20  

#define I2C_STOP_WAIT_VALUE 20  

void I2C_Init();
bit I2C_Put(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char dat);  
bit I2C_Get(unsigned char SlaveAddr, unsigned char SubAddr, unsigned char * dat);  

#endif