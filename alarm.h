#ifndef __CLOCK_ALARM_H__
#define __CLOCK_ALARM_H__

#include "task.h"

void alarm_initialize (void);
void alarm_enter_powersave(void);
void alarm_leave_powersave(void);
void alarm_proc(enum task_events ev);
void alarm_dump(void);

enum alarm0_mode
{
  ALARM0_MOD_PER_SEC                 = 0,
  ALARM0_MOD_MATCH_SEC               = 1,  
  ALARM0_MOD_MATCH_MIN_SEC           = 2, 
  ALARM0_MOD_MATCH_HOUR_MIN_SEC      = 3, 
  ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC = 4,   
  ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  = 5,
};

struct alarm0_struct {
  union {
    unsigned char day;                   // 0-6
    unsigned char date;                  // 0-30(29/28/27)
  }day_date;
  unsigned char hour;                    // 0-23
  unsigned char min;                     // 0-59
  unsigned char sec;                     // 0-59
  unsigned char mode;
};

enum alarm0_mode alarm0_get_mode(void);
void alarm0_set_mode(enum alarm0_mode);
bit alarm0_test_enable(void);
void alarm0_set_enable(bit enable);
bit alarm0_get_hour_12(void);
void alarm0_set_hour_12(bit enable);
unsigned char alarm0_get_sec(void);
void alarm0_inc_sec(void);
unsigned char alarm0_get_min(void);
void alarm0_inc_min(void);
unsigned char alarm0_get_hour(void);
void alarm0_inc_hour(void);
unsigned char alarm0_get_date(void);
void alarm0_inc_date(void);
unsigned char alarm0_get_day(void);
void alarm0_inc_day(void);
void alarm0_sync_from_rtc(void);
void alarm0_sync_to_rtc(void);
bit alarm0_test_hit(void);
void alarm0_clr_hit(void);

struct alarm1_struct {
  union {
    unsigned char day;                   // 0-6
    unsigned char date;                  // 0-30(29/28/27)
  }day_date;
  unsigned char hour;                    // 0-23
  unsigned char min;                     // 0-59
  unsigned char mode;
};

enum alarm1_mode
{
  ALARM1_MOD_PER_MIN                 = 7,
  ALARM1_MOD_MATCH_MIN               = 8,  
  ALARM1_MOD_MATCH_HOUR_MIN          = 9, 
  ALARM1_MOD_MATCH_DATE_HOUR_MIN     = 10,   
  ALARM1_MOD_MATCH_DAY_HOUR_MIN      = 11,     
};

enum alarm1_mode alarm1_get_mode(void);
void alarm1_set_mode(enum alarm1_mode);
bit alarm1_test_enable(void);
void alarm1_set_enable(bit enable);
bit alarm1_get_hour_12(void);
void alarm1_set_hour_12(bit enable);
unsigned char alarm1_get_min(void);
void alarm1_inc_min(void);
unsigned char alarm1_get_hour(void);
void alarm1_inc_hour(void);
unsigned char alarm1_get_date(void);
void alarm1_inc_date(void);
unsigned char alarm1_get_day(void);
void alarm1_sync_from_rtc(void);
void alarm1_sync_to_rtc(void);
bit alarm1_test_hit(void);
void alarm1_clr_hit(void);
#endif