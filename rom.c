#include "rom.h"
#include "debug.h"
#include "i2c.h"

#define ROM_I2C_ADDR 0xA0

unsigned char rom_read(unsigned char addr)
{
  unsigned char dat;
  I2C_Init();
  I2C_Get(ROM_I2C_ADDR, addr, &dat);
  CDBG("rom_read [%02bx] return %bx\n", addr, dat);
  return dat;
}

void rom_write(unsigned char addr, unsigned char val)
{
  I2C_Init();
  I2C_Put(ROM_I2C_ADDR, addr, val);
  CDBG("rom_write [%02bx] with %bx\n", addr, val);
}