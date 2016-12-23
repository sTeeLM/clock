#include "sm_mod_time.h"
#include "led.h"
#include "rtc.h"
#include "clock.h"
#include "debug.h"

#include "mod_common.h"

static void inc_only(unsigned char what)
{
  switch (what) {
    case IS_HOUR:
    case IS_YEAR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(5);
        led_clr_blink(4); 
      }
      if(what == IS_HOUR) {
        clock_inc_hour();
      } else {
        clock_inc_year();
      }
      break;
    case IS_MIN:
    case IS_MON:
      if(!lpress_lock_month_min) {
        lpress_lock_month_min = 1;
        led_clr_blink(3);
        led_clr_blink(2); 
      }
      if(what == IS_MIN) {
        clock_inc_min();
      } else {
        clock_inc_month();
      }
      break;
    case IS_SEC:
    case IS_DAY:
      if(!lpress_lock_day_sec) {
        lpress_lock_day_sec = 1;
        led_clr_blink(1);
        led_clr_blink(0); 
      }
      if(what == IS_SEC) {
        clock_clr_sec();
      } else {
        clock_inc_date();
      }
      break;
  }
}

static void write_only(unsigned char what)
{
  unsigned char bs_hour;
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
        // �������ǻ����������㱨ʱ���ӣ�ʱ���ƺ�һ��Сʱ
        bs_hour = (year_hour + 1) % 24;
        rtc_read_data(RTC_TYPE_ALARM1);
        rtc_alarm_set_hour(bs_hour);
        rtc_write_data(RTC_TYPE_ALARM1);
      }
      break;
    case IS_YEAR:
      if(lpress_lock_year_hour == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break;
    case IS_MON:
      if(lpress_lock_month_min == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2);      
      }
      break;  
    case IS_SEC:
      if(lpress_lock_day_sec == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_TIME);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_DAY:
      if(lpress_lock_day_sec == 1) {
        clock_sync_to_rtc(CLOCK_SYNC_DATE);
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
  
  hour = clock_get_hour();
  min  = clock_get_min();
  sec  = clock_get_sec();
  
  // ������:����
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4); 
  
  // �����12Сʱ��ʾ���Ե�һλ���ֵĵ��ʾ��PM��
  if(clock_get_hour_12() && hour > 12) {
    led_set_dp(5);
    hour -= 12;
  } else {
    led_clr_dp(5);
  }
  
  
  CDBG("update_hhmmss %bd:%bd:%bd\n", hour, min, sec);  
  
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

static void update_yymmdd(void)
{
  unsigned char year, mon, date;
  
  
  year = clock_get_year();
  mon  = clock_get_month();
  date  = clock_get_date();
  
  led_set_dp(2);
  led_set_dp(4);

  CDBG("update_yymmdd %bd-%bd-%bd\n", year, mon, date);
  
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


static void enter_yymmdd(unsigned char what) // blink year:0, month:1, day:2
{
  led_clear();
  switch(what) {
    case IS_YEAR:
      led_set_blink(5);
      led_set_blink(4);    
      break;
    case IS_MON:
      led_set_blink(3);
      led_set_blink(2);    
      break;      
    case IS_DAY:
      led_set_blink(1);
      led_set_blink(0);    
      break;      
  }
  update_yymmdd();  
}


void sm_mod_time(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_mod_time %bd %bd %bd\n", from, to, ev);
  
  // ��mod1�����޸�ʱ��ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(1);
    return;
  }    
    
  // �л����޸�ʱ���ģʽ
  if(get_sm_ss_state(from) == SM_MODIFY_TIME_INIT 
    && get_sm_ss_state(to) == SM_MODIFY_TIME_HH
    && ev == EV_KEY_MOD_UP) {
    enter_hhmmss(IS_HOUR);
    return;
  }
  
  // ��mod0�����޸ķ���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MM && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_MIN);
    return;
  }
  
  // ��set0Сʱ++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_HH && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_HOUR);
    return;
  }

  // set1Сʱ����++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_HH && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // seţ��ֹͣ++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_HH && ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
  
  // ÿ250ms��һ��rtc��ˢ���޸�Сʱģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_HH && ev == EV_250MS) {
    update_hhmmss();
    return;
  }
  
  // mod0 �����޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_SS && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_SEC);
    return;
  } 

  // set0 ����++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MM && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MIN);
    return;
  } 
  
  // set1 ���ӳ���++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MM && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }

  // seţ��ֹͣ++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MM && ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
  
  // ÿ250ms��һ��rtc�������޸ķ���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MM && ev == EV_250MS) {
    update_hhmmss();
    return;
  }
  
  // mod0�����޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_YY && ev == EV_KEY_MOD_PRESS) {
    enter_yymmdd(IS_YEAR);
    return;
  } 

  // set0������0��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_SS && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_SEC);
    return;
  }

  // ÿ250ms��һ��rtc�������޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_SS && ev == EV_250MS) {
    update_hhmmss();
    return;
  }
  
  // ��mod0�����޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MO && ev == EV_KEY_MOD_PRESS) {
    enter_yymmdd(IS_MON);
    return;
  }
  
  // set0 ��++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_YY && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_YEAR);
    return;
  }
  
  // ����set�����++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_YY && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_YEAR);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  } 

  // ̧��setֹͣ��++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_YY && ev == EV_KEY_SET_UP) {
    write_only(IS_YEAR);
    lpress_start = 0;
    return;
  } 
  
  // ÿ250ms��ȡһ��rtc �������޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_YY && ev == EV_250MS) {
    update_yymmdd();
    return;
  } 

  // mod0 �����޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_DD && ev == EV_KEY_MOD_PRESS) {
    enter_yymmdd(IS_DAY);
    return;
  }
  
  // set0 ��++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MO && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MON);
    return;
  }
  
  // ����set �³���++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MO && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MON);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }
  
  // ̧��setֹͣ��++��д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MO && ev == EV_KEY_SET_UP) {
    write_only(IS_MON);
    lpress_start = 0;
    return;
  }
  
  // ÿ250ms��ȡһ��rtc��ˢ���޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_MO && ev == EV_250MS) {
    update_yymmdd();
    return;
  }
  
  // mod0 �����޸�Сʱģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_HH && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_HOUR);
    return;
  }

  // set0 ��++
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_DD && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_DAY);
    return;
  }
  
  // set1 �ճ���++  
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_DD && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_DAY);
    }
    lpress_start++;
    if(lpress_start == 249) lpress_start = 0;
    return;
  }

  // seţ��ֹͣ��++д��rtc
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_DD && ev == EV_KEY_SET_UP) {
    write_only(IS_DAY);
    lpress_start = 0;
    return;
  }
  
  // ÿ250ms��ȡһ��rtc�������޸���ģʽ
  if(get_sm_ss_state(to) == SM_MODIFY_TIME_DD && ev == EV_250MS) {
    update_yymmdd();
    return;
  }
  
}