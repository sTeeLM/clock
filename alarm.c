#include "alarm.h"
#include "rtc.h"
#include "debug.h"
#include "sm.h"
#include "clock.h"
#include "power.h"

static struct alarm0_struct alarm0;

static bit alarm0_is12;
static bit alarm1_enable;

unsigned char clk_min, clk_sec;

void alarm_proc(enum task_events ev)
{
 
  if(ev == EV_ALARM0) {
    CDBG("ALARM0 HIT!\n");
    run_state_machine(EV_ALARM0);
  }
  
  if(ev == EV_ALARM1) {
    CDBG("ALARM1 HIT!\n");
    run_state_machine(EV_ALARM1);
  } 
}

void scan_alarm(void)
{
  bit alarm0_hit, alarm1_hit;
  rtc_read_data(RTC_TYPE_CTL);
  alarm0_hit = rtc_test_alarm_int_flag(0);
  alarm1_hit = rtc_test_alarm_int_flag(1);
  rtc_clr_alarm_int_flag(0);
  rtc_clr_alarm_int_flag(1);
  rtc_write_data(RTC_TYPE_CTL); 

  if(alarm0_hit) {
    if(alarm0.day_mask & (1 << clock_get_day() - 1)) {
      if(power_test_flag()) {
        power_clr_flag();
      }
      set_task(EV_ALARM0);
      if(alarm1_hit) { // 如果同时响起，整点报时被忽略
        alarm1_hit = 0;
      }
    }
  }
  
  if(alarm1_hit) {
    rtc_read_data(RTC_TYPE_TIME);
    if(rtc_time_get_min() == 0 
      && rtc_time_get_sec() == 0) {
      if(power_test_flag()) {
        power_clr_flag();
      }
      set_task(EV_ALARM1);
    }
  }
}

void alarm_initialize (void)
{
  CDBG("alarm_initialize\n");
  alarm0.day_mask = 0x7F;
  alarm0.hour = 12;
  alarm0.min  = 12;
  alarm0_is12 = 1;
  alarm1_enable = 1;
  alarm_dump();
}

void alarm_switch_on(void)
{
  CDBG("alarm_switch_on\n");
  alarm0_sync_to_rtc();
  alarm1_sync_to_rtc();
  rtc_enable_alarm_int(1,0);
  rtc_enable_alarm_int(1,1);
  rtc_set_lt_timer(0);
}

void alarm_switch_off(void)
{
  CDBG("alarm_switch_off\n");
  rtc_set_lt_timer(1);
  rtc_enable_alarm_int(0,0);
  rtc_enable_alarm_int(0,1);
}

void alarm_dump(void)
{
  CDBG("alarm0.day_mask = %bx\n", alarm0.day_mask);
  CDBG("alarm0.hour = %bd\n", alarm0.hour);
  CDBG("alarm0.min  = %bd\n", alarm0.min);
  CDBG("alarm0.is12 = %s\n",  alarm0_is12 ? "ON" : "OFF");  
  CDBG("alarm1.enable = %s\n",  alarm1_enable ? "ON" : "OFF");  
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

// day  1-7
bit alarm0_test_enable(unsigned char day)
{
  CDBG("alarm0_test_enable %bx %bd!\n", alarm0.day_mask, day);
  return (alarm0.day_mask & (1 << (day - 1))) != 0;
}

// day  1-7
void alarm0_set_enable(unsigned char day, bit enable)
{
  CDBG("alarm0_set_enable %bd %s!\n", day, enable ? "ON" : "OFF");
  day = 1 << (day - 1);
  if(enable) {
    alarm0.day_mask |= day;
  } else {
    alarm0.day_mask &= ~day;
  }
  CDBG("alarm0_set_enable res %bx!\n", alarm0.day_mask);
}

bit alarm0_get_hour_12(void)
{
  return alarm0_is12;
}

void alarm0_set_hour_12(bit enable)
{
  alarm0_is12 = enable;
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

void alarm0_sync_from_rtc(void)
{
  CDBG("alarm0_sync_from_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM0);
  alarm0.hour = rtc_alarm_get_hour();
  alarm0.min  = rtc_alarm_get_min();
  alarm0_is12 = rtc_alarm_get_hour_12();
  
}

void alarm0_sync_to_rtc(void)
{
  CDBG("alarm0_sync_to_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_mode(RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC);
  rtc_alarm_set_hour(alarm0.hour);
  rtc_alarm_set_min(alarm0.min);
  rtc_alarm_set_sec(0);
  rtc_alarm_set_hour_12(alarm0_is12);
  rtc_write_data(RTC_TYPE_ALARM0);
  
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(1, 0);
  rtc_clr_alarm_int_flag(0);
  rtc_write_data(RTC_TYPE_CTL);
}

bit alarm1_test_enable(void)
{
  return alarm1_enable;
}

void alarm1_set_enable(bit enable)
{
  alarm1_enable = enable;
}

void alarm1_sync_from_rtc(void)
{
  CDBG("alarm1_sync_from_rtc!\n");
  rtc_read_data(RTC_TYPE_CTL);
  alarm1_enable = rtc_test_alarm_int(1);
}

void alarm1_sync_to_rtc(void)
{
  CDBG("alarm1_sync_to_rtc!\n");
  rtc_read_data(RTC_TYPE_ALARM1);
  rtc_alarm_set_mode(RTC_ALARM1_MOD_MATCH_MIN);
  rtc_alarm_set_min(0);
  rtc_write_data(RTC_TYPE_ALARM1);
  
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(alarm1_enable, 1);
  rtc_clr_alarm_int_flag(1);
  rtc_write_data(RTC_TYPE_CTL);
}