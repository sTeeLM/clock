#include <STC89C5xRC.H>

#include "rom.h"
#include "debug.h"
#include "i2c.h"

#define ROM_I2C_ADDR 0xA0

//sbit ROM_RESET = P3 ^ 7;

sbit ROM_RESET = P2 ^ 6; // 和Mod公用一个，开机时长按Mod相当于工厂reset

unsigned char rom_read(unsigned char addr)
{
  unsigned char dat;
  I2C_Get(ROM_I2C_ADDR, addr, &dat);
  CDBG("rom_read [0x%02bx] return 0x%02bx\n", addr, dat);
  return dat;
}

void rom_write(unsigned char addr, unsigned char val)
{
  I2C_Put(ROM_I2C_ADDR, addr, val);
  CDBG("rom_write [0x%02bx] with 0x%02bx\n", addr, val);
}

static void rom_dump(void)
{
  rom_read(ROM_ALARM0_DAY_MASK);
  rom_read(ROM_ALARM0_HOUR);
  rom_read(ROM_ALARM0_MIN);
  rom_read(ROM_ALARM0_DUR);
  rom_read(ROM_ALARM1_ENABLE);
  
  rom_read(ROM_TIME_IS12);
  rom_read(ROM_BEEPER_MUSIC_INDEX);
  rom_read(ROM_BEEPER_ENABLE);
  rom_read(ROM_POWERSAVE_TO);
  rom_read(ROM_REMOTE_ONOFF);
  
  rom_read(ROM_FUSE_HG_ONOFF);
  rom_read(ROM_FUSE_MPU);
  rom_read(ROM_FUSE_THERMO_HI);
  rom_read(ROM_FUSE_THERMO_LO);
  rom_read(ROM_FUSE_REMOTE_ONOFF);
  
  rom_read(ROM_FUSE_PASSWORD);
  rom_read(ROM_FUSE_PASSWORD + 1);
  rom_read(ROM_FUSE_PASSWORD + 2);
  rom_read(ROM_FUSE_PASSWORD + 3);
  rom_read(ROM_FUSE_PASSWORD + 4);
  rom_read(ROM_FUSE_PASSWORD + 5);
  
  rom_read(ROM_LT_TIMER_YEAR);
  rom_read(ROM_LT_TIMER_MONTH);
  rom_read(ROM_LT_TIMER_DATE);
  rom_read(ROM_LT_TIMER_HOUR);
  rom_read(ROM_LT_TIMER_MIN);
  rom_read(ROM_LT_TIMER_SEC); 
  
  rom_read(ROM_POWER_FULL_INT);
  rom_read(ROM_POWER_FULL_EXP);
  rom_read(ROM_POWER_EMPTY_INT);
  rom_read(ROM_POWER_EMPTY_EXP);
  
  rom_read(ROM_RADIO_FREQ_HI);
  rom_read(ROM_RADIO_FREQ_LO);
  rom_read(ROM_RADIO_VOLUME);
  rom_read(ROM_RADIO_HLSI);
  rom_read(ROM_RADIO_MS);
  rom_read(ROM_RADIO_BL);
  rom_read(ROM_RADIO_HCC);
  rom_read(ROM_RADIO_SNC);
  rom_read(ROM_RADIO_DTC);
}

static void rom_reset(void)
{
  rom_write(ROM_ALARM0_DAY_MASK, 0x7F);
  rom_write(ROM_ALARM0_HOUR, 12);
  rom_write(ROM_ALARM0_MIN, 12);
  rom_write(ROM_ALARM0_DUR, 5);
  rom_write(ROM_ALARM1_ENABLE, 1);
  
  rom_write(ROM_TIME_IS12, 1);
  rom_write(ROM_BEEPER_MUSIC_INDEX, 0);
  rom_write(ROM_BEEPER_ENABLE, 1);
  //0=off 1=15s 2=30s 
  rom_write(ROM_POWERSAVE_TO, 1);
  rom_write(ROM_REMOTE_ONOFF, 1);
  
  rom_write(ROM_FUSE_HG_ONOFF, 1);
  rom_write(ROM_FUSE_MPU, 1);
  rom_write(ROM_FUSE_THERMO_HI, 40);
  rom_write(ROM_FUSE_THERMO_LO, 216);
  rom_write(ROM_FUSE_REMOTE_ONOFF, 1);
  
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
  
  rom_write(ROM_POWER_FULL_INT, 4);
  rom_write(ROM_POWER_FULL_EXP, 20);
  rom_write(ROM_POWER_EMPTY_INT, 3);
  rom_write(ROM_POWER_EMPTY_EXP, 30);
  
  rom_write(ROM_RADIO_FREQ_HI, 0);
  rom_write(ROM_RADIO_FREQ_LO, 0);
  rom_write(ROM_RADIO_VOLUME, 50);
  rom_write(ROM_RADIO_HLSI, 0);
  rom_write(ROM_RADIO_MS, 0);
  rom_write(ROM_RADIO_BL, 0);
  rom_write(ROM_RADIO_HCC, 0);
  rom_write(ROM_RADIO_SNC, 0);
  rom_write(ROM_RADIO_DTC, 0);
}

bit rom_is_factory_reset(void)
{
  return ROM_RESET == 0;
}

void rom_initialize(void)
{
  CDBG("rom_initialize ROM_RESET = %bu(%s)\n", ROM_RESET == 1 ? 1 : 0, ROM_RESET == 1? "OFF" : "ON");
  if(rom_is_factory_reset()) { // reset rom
    CDBG("reset rom!\n");
    rom_reset();
  }
  
  rom_dump();
}
