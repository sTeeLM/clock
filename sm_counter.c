#include "sm_counter.h"
#include "timer.h"
#include "mod_common.h"
#include "debug.h"
#include "led.h"

static void inc_only(unsigned char what)
{
  switch (what) {
    case IS_HOUR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(5);
        led_clr_blink(4); 
      }
      timer_inc_hour(0);
      break;
    case IS_MIN:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(3);
        led_clr_blink(2); 
      }
      timer_inc_min(0);
      break;
    case IS_SEC:
      if(!lpress_lock_day_sec) {
        lpress_lock_day_sec = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
      }
      timer_inc_sec(0);
      break;
  }
}

static void write_only(unsigned char what)
{
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break; 
    case IS_SEC:
      if(lpress_lock_day_sec == 1) {
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;  
  }
}

static void inc_and_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}

static void update_hhmmss(void)
{
  unsigned char hour, min, sec;
  
  hour = timer_get_hour(0);
  min  = timer_get_min(0);
  sec  = timer_get_sec(0);
  
  // 两个“:”号
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4); 
  
  CDBG("update_hhmmss %bd:%bd:%bd\n", hour, min, sec);  
  
  led_set_code(5, (hour / 10) + 0x30);
  led_set_code(4, (hour % 10) + 0x30);
  led_set_code(3, (min / 10)+ 0x30);
  led_set_code(2, (min % 10) + 0x30);
  led_set_code(1, (sec / 10) + 0x30);
  led_set_code(0, (sec % 10) + 0x30);

}

static void enter_hhmmss(unsigned char what) // blink hour:0, min:1, sec:2
{
  led_clear();
  switch(what) {
    case IS_HOUR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MIN:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_SEC:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  update_hhmmss();
}

void sm_counter(unsigned char from, unsigned char to, enum task_events ev)
{

  CDBG("sm_counter %bd %bd %bd\n", from, to, ev);

  // 按set1进入倒计时大模式
  if(get_sm_ss_state(to) == SM_COUNTER_INIT && ev == EV_KEY_SET_LPRESS) {
    display_logo(5);
    return;
  }
  
  // 切换到倒计时大模式
  if(get_sm_ss_state(from) == SM_COUNTER_INIT 
    && get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH
    && ev == EV_KEY_SET_UP) {
    enter_hhmmss(IS_HOUR);
    timer_clr();
    timer_set_mode(TIMER_MODE_DEC);
    timer_set_led_autorefresh(0, TIMER_DISP_MODE_HHMMSS);
    return;
  }
  
  // 刷新显示
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH && ev == EV_250MS) {
    update_hhmmss();
    return;
  } 
  
  // set0小时++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_HOUR);
    return;
  }
  
   // set1小时快速++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // set抬起小时快速++停止
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH && ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
  
  // 刷新显示
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_MM && ev == EV_250MS) {
    update_hhmmss();
    return;
  }   
  
  // mod0进入修改分钟状态
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_MM && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_MIN);
    return;
  }  
  
  // set0 分钟++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_MM && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MIN);
    return;
  }
  
  // set1 分钟快速++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_MM && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // set 抬起分钟快速++停止  
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_MM && ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
  
  // 刷新显示
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_SS && ev == EV_250MS) {
    update_hhmmss();
    return;
  }    
  
  // mod0进入修改秒状态
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_SS && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_SEC);
    return;
  }  
  
  // set0 秒++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_SS && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_SEC);
    return;
  }
  
  // set1 秒快速++
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_SS && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_SEC);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // set抬起秒快速++停止
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_SS && ev == EV_KEY_SET_UP) {
    write_only(IS_SEC);
    lpress_start = 0;
    return;
  }
  
  // mod0 开始倒计时
  if(get_sm_ss_state(to) == SM_COUNTER_RUNNING && ev == EV_KEY_MOD_PRESS) {
    timer_set_led_autorefresh(1, TIMER_DISP_MODE_HHMMSS);
    led_clr_blink(1);
    led_clr_blink(0); 
    timer_start();
    return;
  }
  
  // set0 暂停倒计时
  if(get_sm_ss_state(to) == SM_COUNTER_STOP && ev == EV_KEY_SET_PRESS) {
    timer_stop();
    timer_set_led_autorefresh(0, TIMER_DISP_MODE_HHMMSS);
    return;
  }
  
  // set0 继续倒计时
  if(get_sm_ss_state(to) == SM_COUNTER_RUNNING && ev == EV_KEY_SET_PRESS) {
    timer_set_led_autorefresh(1, TIMER_DISP_MODE_HHMMSS);
    timer_start();
    return;
  }
  
  // mod0 清除
  if(get_sm_ss_state(to) == SM_COUNTER_MODIFY_HH && ev == EV_KEY_MOD_PRESS) {
    timer_set_led_autorefresh(0, TIMER_DISP_MODE_HHMMSS);
    timer_clr();
    return;
  }
}
