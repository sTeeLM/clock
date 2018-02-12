#include "sm_clock_display.h"
#include "led.h"
#include "rtc.h"
#include "clock.h"
#include "power.h"
#include "debug.h"
#include "beeper.h"
#include "cext.h"
#include "mod_common.h"
#include "alarm.h"
#include "lt_timer.h"

#define SM_CLOCK_DISPLAY_SWITCH_S 3 // 5s

const char * code sm_clock_display_ss_name[] = 
{
  "SM_CLOCK_DISPLAY_INIT",
  "SM_CLOCK_DISPLAY_HHMMSS",
  "SM_CLOCK_DISPLAY_YYMMDD",
  "SM_CLOCK_DISPLAY_WEEK",
  "SM_CLOCK_DISPLAY_TEMP",
  NULL
};

static void display_temp(void)
{
  unsigned char inti, flt;
  bit sign;
  
  rtc_read_data(RTC_TYPE_TEMP);
  sign = rtc_get_temperature(&inti, &flt);
  
  CDBG("display_temp %c%bd.%bd\n", sign? '-':'+', inti, flt);
  
  led_clear();
  
  led_set_dp(2);
  
  led_set_code(5, 'C');
  led_set_code(4, sign? '-':LED_CODE_BLACK);
  led_set_code(3, (inti / 10) + 0x30);
  led_set_code(2, (inti % 10) + 0x30);  
  led_set_code(1, (flt / 10) + 0x30);
  led_set_code(0, (flt % 10) + 0x30);    
}

static void reset_auto_switch(void)
{
  last_display_s = clock_get_sec_256();
}


static void test_autoswitch(void)
{
  if(time_diff_now(last_display_s) >= SM_CLOCK_DISPLAY_SWITCH_S) {
    CDBG("test_autoswitch time out!\n");
    set_task(EV_KEY_SET_PRESS);
  }
}

void sm_clock_display(unsigned char from, unsigned char to, enum task_events ev)
{

  CDBG("sm_clock_display %bd %bd %bd\n", from, to, ev);

  // 按mod1进入显示时间大模式
  if(get_sm_ss_state(to) == SM_CLOCK_DISPLAY_INIT && ev == EV_KEY_MOD_LPRESS) {
    lt_timer_switch_off();
    alarm_switch_on();
    rtc_set_lt_timer(0);
    display_logo(DISPLAY_LOGO_TYPE_CLOCK, 0);
    return;
  }
  
  // 切换到时间显示大模式
  if(get_sm_ss_state(from) == SM_CLOCK_DISPLAY_INIT 
    && get_sm_ss_state(to) == SM_CLOCK_DISPLAY_HHMMSS
    && ev == EV_KEY_MOD_UP) {
    led_clear();
    clock_display(1);
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_HHMMSS);
    power_reset_powersave_to();
    return;
  }
    
  // 切换回时分秒显示，从小模式切过来，或者从pac切过来
  if(get_sm_ss_state(to) == SM_CLOCK_DISPLAY_HHMMSS && 
    (ev == EV_KEY_SET_PRESS || ev == EV_KEY_MOD_PRESS)) {
    clock_display(1);
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_HHMMSS);
    power_reset_powersave_to();
    return;
  }  
  
  // 1S探测下睡眠超时时间
  if(get_sm_ss_state(from) == SM_CLOCK_DISPLAY_HHMMSS 
    && get_sm_ss_state(to) == SM_CLOCK_DISPLAY_HHMMSS
    && ev == EV_1S) {
    power_test_powersave_to();
    return;
  }

  // 切换到显示年月日
  if(get_sm_ss_state(to) == SM_CLOCK_DISPLAY_YYMMDD && ev == EV_KEY_MOD_PRESS) {
    //display_yymmdd();
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_YYMMDD);
    reset_auto_switch();
    power_reset_powersave_to();
    return;
  } 
  
  // 1S探测下自动切回时间
  if(get_sm_ss_state(from) == SM_CLOCK_DISPLAY_YYMMDD 
    && get_sm_ss_state(to) == SM_CLOCK_DISPLAY_YYMMDD
    && ev == EV_1S) {
    test_autoswitch();
    return;
  }
  
  // 切换到显示周几
  if(get_sm_ss_state(to) == SM_CLOCK_DISPLAY_WEEK && ev == EV_KEY_MOD_PRESS) {
    //display_week();
    clock_switch_display_mode(CLOCK_DISPLAY_MODE_WEEK);
    reset_auto_switch();
    power_reset_powersave_to();
    return;
  }
  
  // 1S探测下自动切回时间
  if(get_sm_ss_state(from) == SM_CLOCK_DISPLAY_WEEK 
    && get_sm_ss_state(to) == SM_CLOCK_DISPLAY_WEEK
    && ev == EV_1S) {
    test_autoswitch();
    return;
  }  
  
  // 切换到显示温度
  if(get_sm_ss_state(to) == SM_CLOCK_DISPLAY_TEMP && ev == EV_KEY_MOD_PRESS) {
    clock_display(0);
    display_temp();
    reset_auto_switch();
    power_reset_powersave_to();
    return;
  }  
   
  // 1S探测下自动切回时间
  if(get_sm_ss_state(from) == SM_CLOCK_DISPLAY_TEMP 
    && get_sm_ss_state(to) == SM_CLOCK_DISPLAY_TEMP
    && ev == EV_1S) {
    test_autoswitch();
    return;
  }

}