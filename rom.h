#ifndef __CLOCK_ROM_H__
#define __CLOCK_ROM_H__


/*
alarm£º
00  alarm0.day_mask = 0x7F;
01  alarm0.hour = 12;
02  alarm0.min  = 12;
03  alarm0_is12 = 1;
04  alarm1_enable = 1;

beeper£º
05  beeper_music_index
06  beep_enable

power:
07  powersave_to_s
*/

#define ROM_ALARM0_DAY_MASK 0
#define ROM_ALARM0_HOUR     1
#define ROM_ALARM0_MIN      2
#define ROM_ALARM0_IS12     3
#define ROM_ALARM1_ENABLE   4

#define ROM_BEEPER_MUSIC_INDEX 5
#define ROM_BEEPER_ENABLE      6

#define ROM_POWERSAVE_TO    7

#define ROM_FUSE_HG_ONOFF   8
#define ROM_FUSE_GYRO_ONOFF 9
#define ROM_FUSE_THERMO_HI  10
#define ROM_FUSE_THERMO_LO  11
#define ROM_FUSE_TRIPWIRE_ONOFF  12
#define ROM_FUSE_PASSWORD   13
// 13 - 18£¬ 6×Ö½ÚÊÇpassword

#define ROM_LT_TIMER_SYNC_YEAR 19
#define ROM_LT_TIMER_SYNC_MON  20
#define ROM_LT_TIMER_SYNC_DATE 21
#define ROM_LT_TIMER_SYNC_HOUR 22
#define ROM_LT_TIMER_SYNC_MIN  23
#define ROM_LT_TIMER_SYNC_SEC  24

unsigned char rom_read(unsigned char addr);
void rom_write(unsigned char addr, unsigned char val);

#endif