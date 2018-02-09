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
#define LPRESS_INC_OVERFLOW 50

extern bit lpress_lock_year_hour;
extern bit lpress_lock_month_min;
extern bit lpress_lock_day_sec;
extern unsigned char lpress_start;
extern unsigned char last_display_s;
extern unsigned char common_state;
enum display_logo_type {
  DISPLAY_LOGO_TYPE_CLOCK = 0,
  DISPLAY_LOGO_TYPE_FUSE  = 1,  
};

void display_logo(enum display_logo_type type, unsigned char num);


enum param_error
{
  PARAM_ERROR_GYRO_BAD  = 501,
  PARAM_ERROR_NEED_GYRO  = 502,
  PARAM_ERROR_LT_TIMER_OVERFLOW = 503,
  PARAM_ERROR_HG_BAD = 504,
  PARAM_ERROR_TRIPWIRE_BAD = 505,
  PARAM_ERROR_THERMO_HI_BAD = 506,
  PARAM_ERROR_THERMO_TOO_HI = 507,
  PARAM_ERROR_THERMO_LO_BAD = 508,
  PARAM_ERROR_THERMO_TOO_LOW = 509,
  PARAM_ERROR_THERMO_HI_LESS_LO = 510,
  PARAM_ERROR_FUSE_ERROR = 511,
};

void display_param_error(unsigned int err);

#endif