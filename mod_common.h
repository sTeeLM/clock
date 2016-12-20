#ifndef __CLOCK_MOD_COMMON_H__
#define __CLOCK_MOD_COMMON_H__

#define IS_HOUR  0
#define IS_MIN   1
#define IS_SEC   2
#define IS_YEAR  3
#define IS_MON   4
#define IS_DAY   5
#define IS_ONOFF 6

#define LPRESS_INC_DELAY 5

extern unsigned char year_hour;
extern unsigned char month_min;
extern unsigned char day_sec;

extern bit lpress_lock_year_hour;
extern bit lpress_lock_month_min;
extern bit lpress_lock_day_sec;
extern unsigned char lpress_start;

#endif