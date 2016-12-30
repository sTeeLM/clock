#include "sm_display.h"
#include "led.h"
#include "rtc.h"
#include "clock.h"
#include "power.h"
#include "debug.h"
#include "beeper.h"
#include "cext.h"
#include "mod_common.h"

#define SM_DISPLAY_SWITCH_S 3 // 5s

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

static void display_week(void)
{
  unsigned char day;

  day = clock_get_day();

  CDBG("display_week %bd\n", day); 

  led_clear();
  
  led_set_code(5, 'D');
  led_set_code(4, 'A');
  led_set_code(3, 'Y');
  led_set_code(2, '-');  
  led_set_code(1, (day / 10) + 0x30);
  led_set_code(0, (day % 10) + 0x30);    
}

static void display_yymmdd(void)
{
  unsigned char year, mon, date;
  
  year = clock_get_year();
  mon  = clock_get_month();
  date  = clock_get_date();
  
  CDBG("display_yymmdd %bd-%bd-%bd\n", year, mon, date);
  
  led_clear();
  
  led_set_dp(2);
  led_set_dp(4);
  
  if((year / 10) != 0) {
    led_set_code(5, (year / 10) + 0x30);
  } else {
    led_set_code(5, LED_CODE_BLACK);
  }
  led_set_code(4, (year % 10) + 0x30);
  led_set_code(3, (mon / 10)+ 0x30);
  led_set_code(2, (mon % 10) + 0x30);
  led_set_code(1, (date / 10) + 0x30);
  led_set_code(0, (date % 10) + 0x30);
}

static void display_hhmmss(void)
{
  unsigned char hour, min, sec;
  
  hour = clock_get_hour();
  min  = clock_get_min();
  sec  = clock_get_sec();
  
  CDBG("display_hhmmss %bd:%bd:%bd\n", hour, min, sec);
  
  led_clear();
  
  // 两个“:”号
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4);
  
  // 如果是12小时显示，以第一位数字的点表示“PM”
  if(clock_get_hour_12() && hour > 12) {
    led_set_dp(5);
    hour -= 12;
  }
  
  if((hour / 10) != 0) {
    led_set_code(5, (hour / 10) + 0x30);
  } else {
    led_set_code(5, LED_CODE_BLACK);
  }
  led_set_code(4, (hour % 10) + 0x30);
  led_set_code(3, (min / 10)+ 0x30);
  led_set_code(2, (min % 10) + 0x30);
  led_set_code(1, (sec / 10) + 0x30);
  led_set_code(0, (sec % 10) + 0x30);
}

static void reset_switch(void)
{
  last_display_s = clock_get_sec();
}


static void test_autoswitch(void)
{
  if(time_diff(clock_get_sec(), last_display_s) >= SM_DISPLAY_SWITCH_S) {
    CDBG("test_autoswitch time out!\n");
    set_task(EV_KEY_SET_PRESS);
  }
}

void sm_display(unsigned char from, unsigned char to, enum task_events ev)
{

  CDBG("sm_display %bd %bd %bd\n", from, to, ev);

  // 按mod1进入显示时间大模式
  if(get_sm_ss_state(to) == SM_DISPLAY_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(0);
    return;
  }
  
  // 切换到时间显示大模式
  if(get_sm_ss_state(from) == SM_DISPLAY_INIT 
    && get_sm_ss_state(to) == SM_DISPLAY_HHMMSS
    && ev == EV_KEY_MOD_UP) {
    display_hhmmss();
    power_reset_powersave_to();
    return;
  }
  
  // 刷新时分秒显示
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && ev == EV_250MS) {
    display_hhmmss();
    power_test_powersave_to();
    return;
  }

  // 切换到显示年月日
  if(get_sm_ss_state(to) == SM_DISPLAY_YYMMDD && ev == EV_KEY_MOD_PRESS) {
    display_yymmdd();
    reset_switch();
    power_reset_powersave_to();
    return;
  } 
  
  // 切换到显示周几
  if(get_sm_ss_state(to) == SM_DISPLAY_WEEK && ev == EV_KEY_MOD_PRESS) {
    display_week();
    reset_switch();;
    power_reset_powersave_to();
    return;
  }
  
  // 刷新年月日显示
  if(get_sm_ss_state(to) == SM_DISPLAY_YYMMDD && ev == EV_1S) {
    display_yymmdd();
    test_autoswitch();
    power_test_powersave_to();
    return;
  }
  
  // 切换回时分秒显示，从小模式切过来，或者从pac切过来
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && 
    (ev == EV_KEY_SET_PRESS || ev == EV_KEY_MOD_PRESS)) {
    beeper_stop_music();
    display_hhmmss();
    power_reset_powersave_to();
    return;
  }
  
  // 切换到显示温度
  if(get_sm_ss_state(to) == SM_DISPLAY_TEMP && ev == EV_KEY_MOD_PRESS) {
    display_temp();
    reset_switch();;
    power_reset_powersave_to();
    return;
  }  
  
  // 刷新周显示
  if(get_sm_ss_state(to) == SM_DISPLAY_WEEK && ev == EV_1S) {
    display_week();
    test_autoswitch();
    power_test_powersave_to();
    return;
  }  
   

  // 切换回时分秒显示
  if(get_sm_ss_state(to) == SM_DISPLAY_HHMMSS && ev == EV_KEY_MOD_PRESS) {
    display_hhmmss();
    power_reset_powersave_to();
    return;
  }  

  // 刷新温度显示
  if(get_sm_ss_state(to) == SM_DISPLAY_TEMP && ev == EV_1S) {
    display_temp();
    test_autoswitch();
    power_test_powersave_to();    
    return;
  }
}