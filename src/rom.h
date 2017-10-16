#ifndef __CLOCK_ROM_H__
#define __CLOCK_ROM_H__


/*
alarm��
00  alarm0.day_mask = 0x7F;
01  alarm0.hour = 12;
02  alarm0.min  = 12;
03  alarm0_is12 = 1;
04  alarm1_enable = 1;

beeper��
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
#define ROM_BEEPER_MUSIC_TO  7

#define ROM_POWERSAVE_TO    8

#define ROM_FUSE_HG_ONOFF   9
#define ROM_FUSE_GYRO_ONOFF 10
#define ROM_FUSE_THERMO_HI  11
#define ROM_FUSE_THERMO_LO  12
#define ROM_FUSE_TRIPWIRE_ONOFF  13
#define ROM_FUSE_PASSWORD   14
// 14 - 19�� 6�ֽ���password



#define ROM_LT_TIMER_YEAR 20
#define ROM_LT_TIMER_MONTH  21
#define ROM_LT_TIMER_DATE  22
#define ROM_LT_TIMER_HOUR 23
#define ROM_LT_TIMER_MIN  24
#define ROM_LT_TIMER_SEC  25

// 40 ��ʼ�������е�����Ĳ��Խ��
#define ROM_FUSE0_SHORT_GOOD 40
#define ROM_FUSE0_BROKE_GOOD 41
#define ROM_FUSE1_SHORT_GOOD 42
#define ROM_FUSE1_BROKE_GOOD 43
#define ROM_TRIPWIRE_GOOD    44
#define ROM_THERMO_HI_GOOD   45
#define ROM_THERMO_LO_GOOD   46
#define ROM_HG_GOOD          47
#define ROM_GYRO_GOOD        48

unsigned char rom_read(unsigned char addr);
void rom_write(unsigned char addr, unsigned char val);
void rom_initialize(void);
#endif