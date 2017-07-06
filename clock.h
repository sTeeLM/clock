#ifndef __CLOCK_CLOCK_H__
#define __CLOCK_CKOCK_H__

void clock_initialize ();
void clock_enter_powersave(void);
void clock_leave_powersave(void);

enum clock_sync_type
{
  CLOCK_SYNC_TIME = 0,
  CLOCK_SYNC_DATE = 1
};

struct clock_struct 
{
  unsigned char year;   // 0 - 99 (2000 ~ 2099)
  unsigned char mon;    // 0 - 11
  unsigned char date;   // 0 - 30(29/28/27)
  unsigned char day;    // 0 - 6
  unsigned char hour;   // 0 - 23
  unsigned char min;    // 0 - 59
  unsigned char sec;    // 0 - 59
  unsigned char ms39;   // 0 - 255
};

#define CLOCK_YEAR_BASE 2000

bit clock_get_hour_12();
void clock_set_hour_12(bit enable);
unsigned char clock_get_sec(void);
unsigned char clock_get_sec_256(void);
void clock_clr_sec(void);
unsigned char clock_get_min(void);
void clock_inc_min(void);
unsigned char clock_get_hour(void);
void clock_inc_hour(void);
unsigned char clock_get_date(void);
void clock_inc_date(void);
unsigned char clock_get_day(void);
unsigned char clock_get_month(void);
void clock_inc_month(void);
unsigned char clock_get_year(void);
void clock_inc_year(void);
void clock_sync_from_rtc(enum clock_sync_type type);
void clock_sync_to_rtc(enum clock_sync_type type);
void clock_dump(void);

bit clock_is_leap_year(unsigned char year); // year 0-99
unsigned char clock_get_mon_date(unsigned char year, unsigned char mon); // mon 0-11
#endif