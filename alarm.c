#include "alarm.h"
#include "rtc.h"
#include "debug.h"
#include "sm.h"

void alarm_initialize (void)
{
  CDBG("alarm_initialize\n");
  alarm0_sync_from_rtc();
  alarm1_sync_from_rtc();
  alarm_dump();
}

static struct alarm0_struct alarm0;
static struct alarm1_struct alarm1;

static bit alarm0_is12;
static bit alarm1_is12;
static bit alarm0_enable;
static bit alarm1_enable;
static bit alarm0_hit;
static bit alarm1_hit;

void alarm_dump(void)
{
  CDBG("alarm0.mode = %bd\n", alarm0.mode);
  CDBG("alarm0.date = %bd\n", alarm0.day_date.date); 
  CDBG("alarm0.day  = %bd\n", alarm0.day_date.day);
  CDBG("alarm0.hour = %bd\n", alarm0.hour);
  CDBG("alarm0.min  = %bd\n", alarm0.min);
  CDBG("alarm0.sec  = %bd\n", alarm0.sec);
  CDBG("alarm0.is12 = %s\n",  alarm0_is12 ? "ON" : "OFF"); 
  CDBG("alarm0.enable = %s\n",  alarm0_enable ? "ON" : "OFF"); 
  CDBG("alarm0.hit  = %s\n",  alarm0_hit ? "ON" : "OFF"); 
  
  CDBG("alarm1.mode = %bd\n", alarm1.mode);
  CDBG("alarm1.date = %bd\n", alarm1.day_date.date); 
  CDBG("alarm1.day  = %bd\n", alarm1.day_date.day);
  CDBG("alarm1.hour = %bd\n", alarm1.hour);
  CDBG("alarm1.min  = %bd\n", alarm1.min);
  CDBG("alarm1.is12 = %s\n",  alarm1_is12 ? "ON" : "OFF"); 
  CDBG("alarm1.enable = %s\n",  alarm1_enable ? "ON" : "OFF"); 
  CDBG("alarm1.hit  = %s\n",  alarm1_hit ? "ON" : "OFF"); 
}

void alarm_proc(enum task_events ev)
{
  
  // 这里应该判断下如果是在大的时钟模式下
  alarm1_inc_hour();
  alarm1_sync_to_rtc(); // 小时自动++，下一次整点报时  
  
  rtc_read_data(RTC_TYPE_CTL);
  if(rtc_test_alarm_int_flag(0)) {
    CDBG("ALARM0 HIT!\n");
    alarm0_hit = 1;
    rtc_clr_alarm_int_flag(0);
  }
  
  if(rtc_test_alarm_int_flag(1)) {
    CDBG("ALARM1 HIT!\n");
    alarm1_hit = 1;
    rtc_clr_alarm_int_flag(1);
  }
  rtc_write_data(RTC_TYPE_CTL);  
  
  if(alarm0_hit) {
    run_state_machine(EV_ALARM0);
  } 
  if(alarm1_hit) {
    run_state_machine(EV_ALARM1);
  }
  
  // 这里应该判断下如果是在大的时钟模式下
  alarm0_clr_hit();
  alarm1_clr_hit();  
}

void alarm_enter_powersave(void)
{
  CDBG("alarm_enter_powersave!\n");
}

void alarm_leave_powersave(void)
{
  CDBG("alarm_leave_powersave!\n");
  alarm0_sync_from_rtc();
  alarm1_sync_from_rtc();
}

enum alarm0_mode alarm0_get_mode(void)
{
  return alarm0.mode;
}

void alarm0_set_mode(enum alarm0_mode mode)
{
  if(mode >= ALARM0_MOD_PER_SEC && mode <= ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC)
    alarm0.mode = mode;
}

bit alarm0_test_enable(void)
{
  return alarm0_enable;
}

void alarm0_set_enable(bit enable)
{
  alarm0_enable = enable;
}

bit alarm0_get_hour_12(void)
{
  return alarm0_is12;
}

void alarm0_set_hour_12(bit enable)
{
  alarm0_is12 = enable;
}

unsigned char alarm0_get_sec(void)
{
  return alarm0.sec;
}

void alarm0_inc_sec(void)
{
  alarm0.sec = (++alarm0.sec) % 60;
}

unsigned char alarm0_get_min(void)
{
  return alarm0.min;
}

void alarm0_inc_min(void)
{
  alarm0.min = (++ alarm0.min) % 60;
}

unsigned char alarm0_get_hour(void)
{
  return alarm0.hour;
}

void alarm0_inc_hour(void)
{
  alarm0.hour = (++ alarm0.hour) % 24;
}

unsigned char alarm0_get_date(void)
{
  return alarm0.day_date.date;
}

void alarm0_inc_date(void)
{
  alarm0.day_date.date = (++alarm0.day_date.date) % 31;
}

unsigned char alarm0_get_day(void)
{
  return alarm0.day_date.day;
}

void alarm0_inc_day(void)
{
  alarm0.day_date.date = (++alarm0.day_date.day) % 7;
}

void alarm0_sync_from_rtc(void)
{
  CDBG("alarm0_sync_from_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM0);
  alarm0.mode = rtc_alarm_get_mode();
  alarm0.hour = rtc_alarm_get_hour();
  alarm0.min  = rtc_alarm_get_min();
  alarm0.sec  = rtc_alarm_get_sec();
  alarm0_is12 = rtc_alarm_get_hour_12();
  if(alarm0.mode == ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC) {
    alarm0.day_date.date = rtc_alarm_get_date() - 1;
  } else if(alarm0.mode == ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC) {
    alarm0.day_date.day = rtc_alarm_get_day() - 1;
  }
  rtc_read_data(RTC_TYPE_CTL);
  alarm0_enable = rtc_test_alarm_int(0);
  alarm0_hit = rtc_test_alarm_int_flag(0);
  if(alarm1_hit) {
    rtc_clr_alarm_int_flag(0);
  }
}

void alarm0_sync_to_rtc(void)
{
  CDBG("alarm0_sync_to_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_mode(alarm0.mode);
  rtc_alarm_set_hour(alarm0.hour);
  rtc_alarm_set_min(alarm0.min);
  rtc_alarm_set_sec(alarm0.sec);
  rtc_alarm_set_hour_12(alarm0_is12);
  if(alarm0.mode == ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC) {
    rtc_alarm_set_date(alarm0.day_date.date + 1);
  } else if(alarm0.mode == ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC) {
    rtc_alarm_set_day(alarm0.day_date.day + 1);
  }
  rtc_write_data(RTC_TYPE_ALARM0);
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(alarm0_enable, 0);
  rtc_write_data(RTC_TYPE_CTL);
}

bit alarm0_is_hit(void)
{
  return alarm0_hit;
}


void alarm0_clr_hit(void)
{
  alarm0_hit = 0;
}

enum alarm1_mode alarm1_get_mode(void)
{
  return alarm1.mode;
}

void alarm1_set_mode(enum alarm1_mode mode)
{
  if(mode >= ALARM1_MOD_PER_MIN && mode <= ALARM1_MOD_MATCH_DAY_HOUR_MIN)
  {
    alarm1.mode = mode;
  }
}

bit alarm1_test_enable(void)
{
  return alarm1_enable;
}

void alarm1_set_enable(bit enable)
{
  alarm1_enable = enable;
}

bit alarm1_get_hour_12(void)
{
  return alarm1_is12;
}

void alarm1_set_hour_12(bit enable)
{
  alarm1_is12 = enable;
}

unsigned char alarm1_get_min(void)
{
  return alarm1.min;
}

void alarm1_inc_min(void)
{
  alarm1.min = (++alarm1.min) % 60;
}

unsigned char alarm1_get_hour(void)
{
  return alarm1.hour;
}

void alarm1_inc_hour(void)
{
  alarm1.hour = (++alarm1.hour) % 24;
}

unsigned char alarm1_get_date(void)
{
  return alarm1.day_date.date;
}

void alarm1_inc_date(void)
{
  if(alarm1.mode == ALARM1_MOD_MATCH_DATE_HOUR_MIN) {
    alarm1.day_date.date = (++alarm1.day_date.date) % 31;
  }
}

unsigned char alarm1_get_day(void)
{
  return alarm1.day_date.day;
}

void alarm1_inc_day(void)
{
  if(alarm1.mode == ALARM1_MOD_MATCH_DAY_HOUR_MIN) {
    alarm1.day_date.date = (++alarm1.day_date.day) % 7;
  }
}


void alarm1_sync_from_rtc(void)
{
  CDBG("alarm1_sync_from_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM1);
  alarm1.mode = rtc_alarm_get_mode();
  alarm1.hour = rtc_alarm_get_hour();
  alarm1.min  = rtc_alarm_get_min();
  alarm1_is12 = rtc_alarm_get_hour_12();
  if(alarm1.mode == ALARM1_MOD_MATCH_DATE_HOUR_MIN) {
    alarm1.day_date.date = rtc_alarm_get_date() - 1;
  } else if(alarm1.mode == ALARM1_MOD_MATCH_DAY_HOUR_MIN) {
    alarm1.day_date.day = rtc_alarm_get_day() - 1;
  }
  rtc_read_data(RTC_TYPE_CTL);
  alarm1_enable = rtc_test_alarm_int(1);
  alarm1_hit = rtc_test_alarm_int_flag(1);
  if(alarm1_hit) {
    rtc_clr_alarm_int_flag(1);
  }
}

void alarm1_sync_to_rtc(void)
{
  CDBG("alarm1_sync_to_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM1);
  rtc_alarm_set_mode(alarm1.mode);
  rtc_alarm_set_hour(alarm1.hour);
  rtc_alarm_set_min(alarm1.min);
  rtc_alarm_set_hour_12(alarm1_is12);
  if(alarm1.mode == ALARM1_MOD_MATCH_DATE_HOUR_MIN) {
    rtc_alarm_set_date(alarm1.day_date.date + 1);
  } else if(alarm0.mode == ALARM1_MOD_MATCH_DAY_HOUR_MIN) {
    rtc_alarm_set_day(alarm1.day_date.day + 1);
  }
  rtc_write_data(RTC_TYPE_ALARM1);
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(alarm1_enable, 1);
  rtc_write_data(RTC_TYPE_CTL);
}

bit alarm1_is_hit(void)
{
  return alarm1_hit;
}


void alarm1_clr_hit(void)
{
  alarm1_hit = 0;
}