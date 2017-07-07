#ifndef __CLOCK_MOD_COMMON_H__
#define __CLOCK_MOD_COMMON_H__

#define IS_HOUR  0
#define IS_MIN   1
#define IS_SEC   2
#define IS_YEAR  3
#define IS_MON   4
#define IS_DAY   5
#define IS_DAY_ONOFF 6
#define IS_HG    7
#define IS_GYRO  8
#define IS_THERMO_HI 9
#define IS_THERMO_LO 10
#define IS_TRIPWIRE 11
#define IS_PASSWORD 12
#define IS_PASSWORD5 (IS_PASSWORD + 5)

#define IS_PS    0
#define IS_BS    1
#define IS_MUSIC 2
#define IS_BEEP  3
#define IS_1224  4

#define IS_ALARM0 0
#define IS_ALARM1 1
#define IS_COUNTER 2

#define LPRESS_INC_DELAY 5
#define LPRESS_INC_OVERFLOW 249

extern bit lpress_lock_year_hour;
extern bit lpress_lock_month_min;
extern bit lpress_lock_day_sec;
extern unsigned char lpress_start;
extern unsigned char last_display_s;

enum display_logo_type {
	DISPLAY_LOGO_TYPE_CLOCK = 0,
	DISPLAY_LOGO_TYPE_FUSE  = 1,	
};

void display_logo(enum display_logo_type type, unsigned char num);

#endif