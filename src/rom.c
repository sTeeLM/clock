#include <STC89C5xRC.H>

#include "rom.h"
#include "debug.h"
#include "i2c.h"

#define ROM_I2C_ADDR 0xA0

sbit ROM_RESET = P3 ^ 7;

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

static void rom_dump(void)
{
  rom_read(ROM_ALARM0_DAY_MASK);
  rom_read(ROM_ALARM0_HOUR);
  rom_read(ROM_ALARM0_MIN);
  rom_read(ROM_ALARM0_IS12);
  rom_read(ROM_ALARM1_ENABLE);
  
  rom_read(ROM_BEEPER_MUSIC_INDEX);
  rom_read(ROM_BEEPER_ENABLE);
  rom_read(ROM_BEEPER_MUSIC_TO);
  
  //0=off 1=15s 2=30s 
  rom_read(ROM_POWERSAVE_TO);
  
  rom_read(ROM_FUSE_HG_ONOFF);
  rom_read(ROM_FUSE_GYRO_ONOFF);
  rom_read(ROM_FUSE_THERMO_HI);
  rom_read(ROM_FUSE_THERMO_LO);
  rom_read(ROM_FUSE_TRIPWIRE_ONOFF);
  
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD);
  
  rom_read(ROM_LT_TIMER_YEAR);
  rom_read(ROM_LT_TIMER_MONTH); // 8月
  rom_read(ROM_LT_TIMER_DATE); // 19日
  rom_read(ROM_LT_TIMER_HOUR);
  rom_read(ROM_LT_TIMER_MIN);
  rom_read(ROM_LT_TIMER_SEC); 
  
  rom_read(ROM_FUSE0_BROKE_GOOD);
  rom_read(ROM_FUSE1_BROKE_GOOD);
  rom_read(ROM_TRIPWIRE_GOOD);
  rom_read(ROM_THERMO_HI_GOOD);
  rom_read(ROM_THERMO_LO_GOOD);
  rom_read(ROM_HG_GOOD);
  rom_read(ROM_GYRO_GOOD);
}

static void rom_reset(void)
{
  rom_write(ROM_ALARM0_DAY_MASK, 0x7F);
  rom_write(ROM_ALARM0_HOUR, 12);
  rom_write(ROM_ALARM0_MIN, 12);
  rom_write(ROM_ALARM0_IS12, 1);
  rom_write(ROM_ALARM1_ENABLE, 1);
  
  rom_write(ROM_BEEPER_MUSIC_INDEX, 0);
  rom_write(ROM_BEEPER_ENABLE, 1);
  rom_write(ROM_BEEPER_MUSIC_TO, 30);
  
  //0=off 1=15s 2=30s 
  rom_write(ROM_POWERSAVE_TO, 1);
  
  rom_write(ROM_FUSE_HG_ONOFF, 1);
  rom_write(ROM_FUSE_GYRO_ONOFF, 1);
  rom_write(ROM_FUSE_THERMO_HI, 40);
  rom_write(ROM_FUSE_THERMO_LO, 216);
  rom_write(ROM_FUSE_TRIPWIRE_ONOFF, 1);
  
  rom_write(ROM_FUSE_PASSWORD,     1);
  rom_write(ROM_FUSE_PASSWORD + 1, 2);
  rom_write(ROM_FUSE_PASSWORD + 2, 3);
  rom_write(ROM_FUSE_PASSWORD + 3, 4);
  rom_write(ROM_FUSE_PASSWORD + 4, 5);
  rom_write(ROM_FUSE_PASSWORD + 5, 6);
  
  rom_write(ROM_LT_TIMER_YEAR, 14);
  rom_write(ROM_LT_TIMER_MONTH, 7); // 8月
  rom_write(ROM_LT_TIMER_DATE, 18); // 19日
  rom_write(ROM_LT_TIMER_HOUR, 12);
  rom_write(ROM_LT_TIMER_MIN, 15);
  rom_write(ROM_LT_TIMER_SEC, 10); 
  
  rom_write(ROM_FUSE0_BROKE_GOOD, 0);
  rom_write(ROM_FUSE1_BROKE_GOOD, 0);
  rom_write(ROM_TRIPWIRE_GOOD, 0);
  rom_write(ROM_THERMO_HI_GOOD, 0);
  rom_write(ROM_THERMO_LO_GOOD, 0);
  rom_write(ROM_HG_GOOD, 0);
  rom_write(ROM_GYRO_GOOD, 0);
}

void rom_initialize(void)
{
  CDBG("rom_initialize ROM_RESET = %bd\n", ROM_RESET == 1 ? 1 : 0);
  if(ROM_RESET == 0) { // reset rom
    CDBG("reset rom!\n");
    rom_reset();
  }
  
  rom_dump();
}