#include "alarm.h"
#include "rtc.h"
#include "debug.h"
#include "sm.h"



void alarm_initialize (void)
{
  CDBG("alarm_initialize\n");
}

void alarm_proc(enum task_events ev)
{
  unsigned char hour;
  bit alarm0_hit;
  bit alarm1_hit;
  
  // 判断是alarm0还是alarm1,清除RTC中的中断标志位
  rtc_read_data(RTC_TYPE_CTL);
  if(rtc_test_alarm_int_flag(0)) {
    alarm0_hit = 1;
    rtc_clr_alarm_int_flag(0);
    CDBG("alarm0 HIT!\n");
  } else if(rtc_test_alarm_int_flag(1)) {
    alarm1_hit = 1;
    rtc_clr_alarm_int_flag(1);
    CDBG("alarm1 HIT!\n");
  }
  rtc_write_data(RTC_TYPE_CTL);
  
  // 如果是alarm1：设置下一个整点报时时间
  if(alarm1_hit) {
    rtc_read_data(RTC_TYPE_ALARM1);
    hour = rtc_alarm_get_hour();
    hour = (hour + 1) % 24;
    rtc_alarm_set_hour(hour);
    rtc_write_data(RTC_TYPE_ALARM1);
  }
  
  // run 状态机
  if(alarm0_hit && alarm1_hit) {
    run_state_machine(EV_ALARM0); // 如果0/1同时hit，忽略1（整点报时）
  } else {
      if(alarm0_hit) {
        run_state_machine(EV_ALARM0);
      } else if(alarm1_hit){
        run_state_machine(EV_ALARM1);
      }
  }
}

void alarm_enter_powersave(void)
{
  
}

void alarm_leave_powersave(void)
{
  
}