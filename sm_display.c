#include "sm_display.h"
#include "led.h"
#include "rtc.h"
#include "debug.h"


static void display_hhmmss(void)
{
  unsigned char hour, min, sec;
  
  rtc_read_data(RTC_TYPE_TIME);
  hour = rtc_time_get_hour();
  min  = rtc_time_get_min();
  sec  = rtc_time_get_sec();
  led_clear();
  
  CDBG("%bd:%bd:%bd\n", hour, min, sec);
  
  // 两个“:”号
  led_set_dp(2);
  led_set_dp(4);

  // 如果是12小时显示，以第一位数字的点表示“PM”
  if(rtc_time_get_hour_12() && hour > 12) {
    led_set_dp(5);
    hour -= 12;
  }
    
  led_set_code(5, (hour / 10) + 0x30);
  led_set_code(4, (hour % 10) + 0x30);
  led_set_code(3, (min / 10)+ 0x30);
  led_set_code(2, (min % 10) + 0x30);
  led_set_code(1, (sec / 10) + 0x30);
  led_set_code(0, (sec % 10) + 0x30);  
}

void sm_display(unsigned char from, unsigned char to, enum task_events ev)
{

  CDBG("sm_display %bd %bd %bd\n", from, to, ev);

  // 空操作
  if(get_sm_ss_state(from) == SM_DISPLAY_INIT && ev == EV_KEY_MOD_UP) {
    return;
  }
     
  // 刷新时分秒显示
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && ev == EV_250MS) {
    display_hhmmss();
    return;
  }
  /*
  // 切换到显示年月日
  if(get_sm_ss_state(to) == SM_DISPLAY_YYMMDD && ev == EV_KEY_MOD_PRESS) {
    return;
  }  
  
  
  // 切换到显示周几
  if(get_sm_ss_state(to) == SM_DISPLAY_WEEK && ev == EV_KEY_MOD_PRESS) {
    return;
  }
  
  // 刷新年月日显示
  if(get_sm_ss_state(to) == SM_DISPLAY_YYMMDD && ev == EV_1S) {
    return;
  }
  
  // 切换回时分秒显示
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && 
    (ev == EV_KEY_SET_PRESS || ev == EV_KEY_MOD_PRESS)) {
    return;
  }
  
  // 切换到显示温度
  if(get_sm_ss_state(to) == SM_DISPLAY_TEMP && ev == EV_KEY_MOD_PRESS) {
    return;
  }  
  
  // 刷新周显示
  if(get_sm_ss_state(to) == SM_DISPLAY_WEEK && ev == EV_1S) {
    display_hhmmss();
    return;
  }  
   

  // 切换回时分秒显示
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && ev == EV_KEY_MOD_PRESS) {
    return;
  }  

  // 刷新温度显示
  if(get_sm_ss_state(to) == SM_DISPLAY_TEMP && ev == EV_1S) {
    return;
  }
  */
}