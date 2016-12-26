#ifndef __CLOCK_MOD_COMMON_H__
#define __CLOCK_MOD_COMMON_H__

#define IS_HOUR  0
#define IS_MIN   1
#define IS_SEC   2
#define IS_YEAR  3
#define IS_MON   4
#define IS_DAY   5
#define IS_DAY_ONOFF 6

#define IS_PS    0
#define IS_BS    1
#define IS_MUSIC 2
#define IS_1224  3

#define IS_ALARM0 0
#define IS_ALARM1 1
#define IS_COUNTER 2

#define LPRESS_INC_DELAY 5

extern bit lpress_lock_year_hour;
extern bit lpress_lock_month_min;
extern bit lpress_lock_day_sec;
extern unsigned char lpress_start;
extern unsigned char last_display_s;

void display_logo(unsigned char fun);

#endif