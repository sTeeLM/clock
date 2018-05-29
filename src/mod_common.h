#ifndef __CLOCK_MOD_COMMON_H__
#define __CLOCK_MOD_COMMON_H__

#define IS_HOUR  0
#define IS_MIN   1
#define IS_SEC   2
#define IS_YEAR  3
#define IS_MON   4
#define IS_DAY   5
#define IS_ONOFF 6
#define IS_TEST  7
#define IS_FUSE0 8
#define IS_FUSE1 9
#define IS_HG    10
#define IS_MPU   11
#define IS_THERMO_HI 12
#define IS_THERMO_LO 13
#define IS_TRIPWIRE 14
#define IS_PASSWORD 15
#define IS_PASSWORD5 (IS_PASSWORD + 5)

#define IS_PS    16
#define IS_BS    17
#define IS_MUSIC 18
#define IS_BEEP  19
#define IS_1224  20

#define IS_ALARM0 21
#define IS_ALARM1 22
#define IS_COUNTER 23

#define LPRESS_INC_DELAY 2
#define LPRESS_INC_OVERFLOW 50

extern bit lpress_lock_year_hour;
extern bit lpress_lock_month_min;
extern bit lpress_lock_day_sec;
extern unsigned char lpress_start;
extern unsigned char last_display_s;
extern unsigned char common_state;
enum display_logo_type {
  DISPLAY_LOGO_TYPE_CLOCK = 0,
	DISPLAY_LOGO_TYPE_POWER_PACK = 1,
  DISPLAY_LOGO_TYPE_FUSE  = 2, 
	DISPLAY_LOGO_TYPE_GLOBAL_FLAG = 3
};

void display_logo(enum display_logo_type type, unsigned char num);

#endif