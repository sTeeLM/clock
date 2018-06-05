#ifndef __CLOCK_ROM_H__
#define __CLOCK_ROM_H__

#define ROM_ALARM0_DAY_MASK 0
#define ROM_ALARM0_HOUR     1
#define ROM_ALARM0_MIN      2
#define ROM_ALARM1_ENABLE   3

#define ROM_TIME_IS12       4
#define ROM_BEEPER_MUSIC_INDEX 5
#define ROM_BEEPER_ENABLE      6
#define ROM_BEEPER_MUSIC_TO  7
#define ROM_POWERSAVE_TO    8

#define ROM_FUSE_HG_ONOFF   9
#define ROM_FUSE_MPU        10
#define ROM_FUSE_THERMO_HI  11
#define ROM_FUSE_THERMO_LO  12
#define ROM_FUSE_PASSWORD   13
// 6字节是password

#define ROM_LT_TIMER_YEAR  20
#define ROM_LT_TIMER_MONTH 21
#define ROM_LT_TIMER_DATE  22
#define ROM_LT_TIMER_HOUR  23
#define ROM_LT_TIMER_MIN   24
#define ROM_LT_TIMER_SEC   25

unsigned char rom_read(unsigned char addr);
void rom_write(unsigned char addr, unsigned char val);
void rom_initialize(void);
bit rom_is_factory_reset(void);
#endif
