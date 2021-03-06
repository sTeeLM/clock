#include <intrins.h>
#include "alarm.h"
#include "rtc.h"
#include "debug.h"
#include "sm.h"
#include "clock.h"
#include "power.h"
#include "rom.h"
#include "radio.h"
#include "delay_task.h"

#define ALARM0_MAX_DUR_MIN 30 // 30分钟

static struct alarm0_struct alarm0;

static bit alarm0_is12;
static bit alarm1_enable;

static bit play_radio_stop;

static unsigned int play_radio_sec_left;

void alarm_proc(enum task_events ev)
{
 
  if(ev == EV_ALARM0) {
    CDBG(("ALARM0 HIT!\n"));
    run_state_machine(EV_ALARM0);
  }
  
  if(ev == EV_ALARM1) {
    CDBG(("ALARM1 HIT!\n"));
    run_state_machine(EV_ALARM1);
  } 
}

void scan_alarm(void)
{
  bit alarm0_hit, alarm1_hit;
  
  rtc_read_data(RTC_TYPE_CTL);
  alarm0_hit = rtc_test_alarm_int_flag(RTC_ALARM0);
  alarm1_hit = rtc_test_alarm_int_flag(RTC_ALARM1);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
  rtc_clr_alarm_int_flag(RTC_ALARM1);
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
  
  // 这段逻辑会被频繁调1S用长达1分钟
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

static void alarm_load_rom(void)
{
  alarm0.day_mask = rom_read(ROM_ALARM0_DAY_MASK);
  alarm0.hour = rom_read(ROM_ALARM0_HOUR);
  alarm0.min  = rom_read(ROM_ALARM0_MIN);
  alarm0.dur  = rom_read(ROM_ALARM0_DUR);
  alarm0_is12 = rom_read(ROM_TIME_IS12);
  alarm1_enable = rom_read(ROM_ALARM1_ENABLE); 
}

void alarm_save_rom(enum alarm_sync_type t)
{
  switch (t) {
    case ALARM_SYNC_ALARM0_DAY_MASK:
      rom_write(ROM_ALARM0_DAY_MASK, alarm0.day_mask);
    break;
    case ALARM_SYNC_ALARM0_HOUR:
      rom_write(ROM_ALARM0_HOUR, alarm0.hour);
    break;
    case ALARM_SYNC_ALARM0_MIN:
      rom_write(ROM_ALARM0_MIN, alarm0.min);
    break;
    case ALARM_SYNC_ALARM0_DUR:
      rom_write(ROM_ALARM0_DUR, alarm0.dur);
    break;
    case ALARM_SYNC_ALARM1_ENABLE:
      rom_write(ROM_ALARM1_ENABLE, alarm1_enable);
    break;
  }
}



void alarm_initialize (void)
{

}

void alarm_switch_on(void)
{
  CDBG(("alarm_switch_on\n"));
  alarm_load_rom();
  alarm_dump();
  alarm0_sync_to_rtc();
  alarm1_sync_to_rtc();
  rtc_dump();
}

void alarm_switch_off(void)
{
  CDBG(("alarm_switch_off\n"));
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, 0);
  rtc_enable_alarm_int(RTC_ALARM1, 0);
  rtc_write_data(RTC_TYPE_CTL);
  rtc_dump();
}

void alarm_dump(void)
{
  CDBG(("alarm0.day_mask = 0x%02bx\n", alarm0.day_mask));
  CDBG(("alarm0.hour = %bu\n", alarm0.hour));
  CDBG(("alarm0.min  = %bu\n", alarm0.min));
  CDBG(("alarm0.is12 = %s\n",  alarm0_is12 ? "ON" : "OFF"));  
  CDBG(("alarm1.enable = %s\n",  alarm1_enable ? "ON" : "OFF"));  
}

void alarm_enter_powersave(void)
{

}

void alarm_leave_powersave(void)
{

}

// day  1-7
bit alarm0_test_enable(unsigned char day)
{
  CDBG(("alarm0_test_enable 0x%02bx %bu!\n", alarm0.day_mask, day));
  return (alarm0.day_mask & (1 << (day - 1))) != 0;
}

// day  1-7
void alarm0_set_enable(unsigned char day, bit enable)
{
  CDBG(("alarm0_set_enable %bu %s!\n", day, enable ? "ON" : "OFF"));
  day = 1 << (day - 1);
  if(enable) {
    alarm0.day_mask |= day;
  } else {
    alarm0.day_mask &= ~day;
  }
  CDBG(("alarm0_set_enable res 0x%02bx!\n", alarm0.day_mask));
  
}

unsigned char alarm0_get_day_mask(void)
{
  return alarm0.day_mask;
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

void alarm0_sync_to_rtc(void)
{
  CDBG(("alarm0_sync_to_rtc!\n"));
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_mode(RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC);
  rtc_alarm_set_hour(alarm0.hour);
  rtc_alarm_set_min(alarm0.min);
  rtc_alarm_set_sec(0);
  rtc_write_data(RTC_TYPE_ALARM0);
  
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, alarm0.day_mask != 0);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
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

void alarm1_sync_to_rtc(void)
{
  CDBG(("alarm1_sync_to_rtc!\n"));
  rtc_read_data(RTC_TYPE_ALARM1);
  rtc_alarm_set_mode(RTC_ALARM1_MOD_MATCH_MIN);
  rtc_alarm_set_hour(0);
  rtc_alarm_set_min(0);
  rtc_write_data(RTC_TYPE_ALARM1);
  
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM1, alarm1_enable);
  rtc_clr_alarm_int_flag(RTC_ALARM1);
  rtc_write_data(RTC_TYPE_CTL);
}

// 5 -> 10 -> 15 -> 20 -> 25 -> 30 -> 5
void alarm0_inc_dur(void)
{
  alarm0.dur += 5;
  if(alarm0.dur > ALARM0_MAX_DUR_MIN) {
    alarm0.dur = 5;
  }
}

unsigned char alarm0_get_dur(void)
{
  return alarm0.dur;
}

void alarm_stop_radio(void)
{
  play_radio_stop = 1;
}

#pragma NOAREGS
void alarm_play_radio_cb(void)
{
  if(play_radio_sec_left == 0)
    return;
  
  if((--play_radio_sec_left ) == 0) {
    play_radio_stop = 1;
  }
}
#pragma AREGS

void alarm_play_radio(void)
{
  radio_enable(1);
  play_radio_stop = 0;
  play_radio_sec_left = alarm0.dur * 60;
  while(!play_radio_stop) {
    _nop_();
    CDBG(("play_radio_sec_left = %u\n", play_radio_sec_left));
  }
  radio_enable(0);
}