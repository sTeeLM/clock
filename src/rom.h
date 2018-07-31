#ifndef __CLOCK_ROM_H__
#define __CLOCK_ROM_H__

#define ROM_ALARM0_DAY_MASK 0
#define ROM_ALARM0_HOUR     1
#define ROM_ALARM0_MIN      2
#define ROM_ALARM0_DUR      3
#define ROM_ALARM1_ENABLE   4

#define ROM_TIME_IS12       10
#define ROM_BEEPER_MUSIC_INDEX 11
#define ROM_BEEPER_ENABLE      12
#define ROM_POWERSAVE_TO    13
#define ROM_REMOTE_ONOFF    14

#define ROM_FUSE_HG_ONOFF   20
#define ROM_FUSE_MPU        21
#define ROM_FUSE_THERMO_HI  22
#define ROM_FUSE_THERMO_LO  23
#define ROM_FUSE_REMOTE_ONOFF 24
#define ROM_FUSE_PASSWORD   25
// 6字节是password

#define ROM_LT_TIMER_YEAR  40
#define ROM_LT_TIMER_MONTH 41
#define ROM_LT_TIMER_DATE  42
#define ROM_LT_TIMER_HOUR  43
#define ROM_LT_TIMER_MIN   44
#define ROM_LT_TIMER_SEC   45

#define ROM_RADIO_FREQ_HI 60
#define ROM_RADIO_FREQ_LO 61
#define ROM_RADIO_VOLUME 62
#define ROM_RADIO_HLSI 63
#define ROM_RADIO_MS  64
#define ROM_RADIO_BL  65
#define ROM_RADIO_HCC 66
#define ROM_RADIO_SNC 67
#define ROM_RADIO_DTC 68

unsigned char rom_read(unsigned char addr);
void rom_write(unsigned char addr, unsigned char val);
void rom_initialize(void);
bit rom_is_factory_reset(void);
#endif
