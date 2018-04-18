#include "sm_fuse_param.h"
#include "debug.h"
#include "mod_common.h"
#include "rom.h"
#include "led.h"
#include "lt_timer.h"
#include "clock.h"
#include "thermo.h"

const char * code sm_fuse_param_ss_name[] = 
{
  "SM_FUSE_PARAM_INIT",
  "SM_FUSE_PARAM_YY",
  "SM_FUSE_PARAM_MO",
  "SM_FUSE_PARAM_DD",
  "SM_FUSE_PARAM_HH",
  "SM_FUSE_PARAM_MM",
  "SM_FUSE_PARAM_SS",
  "SM_FUSE_PARAM_HG_ONOFF",
  "SM_FUSE_PARAM_MPU_ONOFF",
  "SM_FUSE_PARAM_THERMO_HI_ONOFF",
  "SM_FUSE_PARAM_THERMO_LO_ONOFF",
  "SM_FUSE_PARAM_PASSWORD",
  NULL
};

static void inc_only(unsigned char what)
{
  
  if(what >= IS_PASSWORD && what <= IS_PASSWORD5) {
    last_display_s = ++ last_display_s % 10;
    return;
  }
  
  switch (what) {
    case IS_HOUR:
    case IS_YEAR:
      if(!lpress_lock_year_hour) {
        lpress_lock_year_hour = 1;
        led_clr_blink(5);
        led_clr_blink(4); 
      }
      if(what == IS_HOUR) {
        lt_timer_inc_hour();
      } else {
        lt_timer_inc_year();
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
        lt_timer_inc_min();
      } else {
        lt_timer_inc_month();
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
        lt_timer_inc_sec();
      } else {
        lt_timer_inc_date();
      }
      break;
    case IS_HG:
    case IS_MPU:
      last_display_s = !last_display_s;
      break;
    case IS_THERMO_HI:
    case IS_THERMO_LO:
      last_display_s = (unsigned char)thermo_threshold_inc(last_display_s);
      CDBG("thermo %bd %bx\n", last_display_s, last_display_s);
      break;
  }
}

static void write_only(unsigned char what)
{
  unsigned char i;
  
  if(what >= IS_PASSWORD && what <= IS_PASSWORD5) {
    i = what - IS_PASSWORD;
    rom_write(ROM_FUSE_PASSWORD + i, last_display_s);
    return;
  }
  
  switch(what) {
    case IS_HOUR:
      if(lpress_lock_year_hour == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_HOUR);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_MIN:
      if(lpress_lock_month_min == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_MIN);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2); 
      }
      break; 
    case IS_SEC:
      if(lpress_lock_day_sec == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_SEC);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_DAY:
      if(lpress_lock_day_sec == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_DATE);
        lpress_lock_day_sec = 0;
        led_set_blink(1);
        led_set_blink(0); 
      }
      break;
    case IS_MON:
      if(lpress_lock_month_min == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_MONTH);
        lpress_lock_month_min = 0;
        led_set_blink(3);
        led_set_blink(2);      
      }
      break;
    case IS_YEAR:
      if(lpress_lock_year_hour == 1) {
        lt_timer_sync_to_rom(LT_TIMER_SYNC_YEAR);
        lpress_lock_year_hour = 0;
        led_set_blink(5);
        led_set_blink(4);
      }
      break;
    case IS_HG:
      rom_write(ROM_FUSE_HG_ONOFF, last_display_s);
      break;   
    case IS_MPU:
      rom_write(ROM_FUSE_MPU_ONOFF, last_display_s);
      break;
    case IS_THERMO_HI:
      rom_write(ROM_FUSE_THERMO_HI, last_display_s);
      break;
    case IS_THERMO_LO:
      rom_write(ROM_FUSE_THERMO_LO, last_display_s);
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
  
  hour = lt_timer_get_hour();
  min  = lt_timer_get_min();
  sec  = lt_timer_get_sec();
  
  // 两个“:”号
  led_set_dp(1);
  led_set_dp(2);
  led_set_dp(3);
  led_set_dp(4); 
  
  // 如果是12小时显示，以第一位数字的点表示“PM”
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
  led_set_code(3, (min / 10)  + 0x30);
  led_set_code(2, (min % 10) + 0x30);
  led_set_code(1, (sec / 10) + 0x30);
  led_set_code(0, (sec % 10) + 0x30);
}

static void update_yymmdd(void)
{
  unsigned char year, mon, date;
  
  year = lt_timer_get_year();
  mon  = lt_timer_get_month();
  date = lt_timer_get_date();
  
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

static void update_thermo(unsigned char what)
{
  char value;
  led_clear();
  switch(what) {
    case IS_THERMO_HI:
      led_set_code(5, 'H');
      led_set_code(4, 'I');  
      break;
    case IS_THERMO_LO:
      led_set_code(5, 'L');
      led_set_code(4, 'O');  
      break;
  }
  if(last_display_s != THERMO_THRESHOLED_INVALID) {
    value = (char) last_display_s;
    if(value < 0) {
      led_set_code(2, '-');
      value = 0 - value;
    }
    led_set_code(1, value / 10 + 0x30);
    led_set_code(0, value % 10 + 0x30); 
  } else {
    led_set_code(2, 'O');
    led_set_code(1, 'F');
    led_set_code(0, 'F');
  }
}

static void enter_thermo(unsigned char what)
{
  switch(what) {
    case IS_THERMO_HI:
      last_display_s = rom_read(ROM_FUSE_THERMO_HI);
      break;
    case IS_THERMO_LO:
      last_display_s = rom_read(ROM_FUSE_THERMO_LO);
      break;
  }
  update_thermo(what);
}

static void update_onoff(unsigned char what)
{
  led_clear();
  switch(what) {
    case IS_HG:
      led_set_code(5, 'H');
    break;
    case IS_MPU:
      led_set_code(5, 'O');
    break;    
  }
  
  if(last_display_s) {
    led_set_code(1, 'O');
    led_set_code(0, 'N');
  } else {
    led_set_code(2, 'O');
    led_set_code(1, 'F');
    led_set_code(0, 'F');
  }
}

static void enter_onoff(unsigned char what)
{
  switch(what) {
    case IS_HG:
      last_display_s = rom_read(ROM_FUSE_HG_ONOFF);
      break;
    case IS_MPU:
      last_display_s = rom_read(ROM_FUSE_MPU_ONOFF);
      break;
  }
  update_onoff(what);
}

static void update_password(unsigned char index)
{
  unsigned char i;
  i = index - IS_PASSWORD;
  led_clear();
  led_set_code(5, '-');
  led_set_code(4, '-');
  led_set_code(3, '-');
  led_set_code(2, '-');
  led_set_code(1, '-');
  led_set_code(0, '-');
  led_set_code(i, last_display_s + 0x30);
}

static void enter_password(unsigned char index)
{
  unsigned char i;
  i = index - IS_PASSWORD;
  last_display_s = rom_read(ROM_FUSE_PASSWORD + i);
  update_password(index);
}

void sm_fuse_param(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_param %bd %bd %bd\n", from, to, ev);
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_INIT && ev == EV_KEY_SET_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 1);
    return;
  }
  
  // 防止误操作
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_YY
    && (get_sm_ss_state(from) == SM_FUSE_PARAM_INIT || get_sm_ss_state(from) == SM_FUSE_PARAM_PASSWORD)
    && (ev == EV_KEY_SET_UP|| ev == EV_FUSE_SEL0)) {
    lt_timer_sync_from_rom();
    common_state = 5;
    enter_yymmdd(IS_YEAR);
    return;
  }  

  // 调整年
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_YY && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_YEAR);
    update_yymmdd();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_YY && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_YEAR);
      update_yymmdd();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_YY && ev == EV_KEY_SET_UP) {
    write_only(IS_YEAR);
    lpress_start = 0;
    return;
  }   
  
  // 调整月
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MO && ev == EV_KEY_MOD_PRESS) {
    enter_yymmdd(IS_MON);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MO && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MON);
    update_yymmdd();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MO && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MON);
      update_yymmdd();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MO && ev == EV_KEY_SET_UP) {
    write_only(IS_MON);
    lpress_start = 0;
    return;
  } 
  
  // 调整日
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_DD && ev == EV_KEY_MOD_PRESS) {
    enter_yymmdd(IS_DAY);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_DD && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_DAY);
    update_yymmdd();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_DD && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_DAY);
      update_yymmdd();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_DD && ev == EV_KEY_SET_UP) {
    write_only(IS_DAY);
    lpress_start = 0;
    return;
  }  

  // 调整时
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HH && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_HOUR);
    return;
  }
    
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HH && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_HOUR);
    update_hhmmss();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HH && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_HOUR);
      update_hhmmss();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HH && ev == EV_KEY_SET_UP) {
    write_only(IS_HOUR);
    lpress_start = 0;
    return;
  }
  
  // 调整分
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MM && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_MIN);
    return;
  }
    
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MM && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MIN);
    update_hhmmss();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MM && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_MIN);
      update_hhmmss();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MM && ev == EV_KEY_SET_UP) {
    write_only(IS_MIN);
    lpress_start = 0;
    return;
  }
  
  // 调整秒
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_SS && ev == EV_KEY_MOD_PRESS) {
    enter_hhmmss(IS_SEC);
    return;
  }
    
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_SS && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_SEC);
    update_hhmmss();
    return;
  } 
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_SS && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_SEC);
      update_hhmmss();
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_SS && ev == EV_KEY_SET_UP) {
    write_only(IS_SEC);
    lpress_start = 0;
    return;
  }
  
  // 调整hg on/off
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HG_ONOFF && ev == EV_KEY_MOD_PRESS) {
    enter_onoff(IS_HG);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_HG_ONOFF && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_HG);
    update_onoff(IS_HG);
    return;
  }
  
  // 调整mpu on/off
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MPU_ONOFF && ev == EV_KEY_MOD_PRESS) {
    enter_onoff(IS_MPU);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_MPU_ONOFF && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_MPU);
    update_onoff(IS_MPU);
    return;
  }  
  
  // 调整thermo hi
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_HI_ONOFF && ev == EV_KEY_MOD_PRESS) {
    enter_thermo(IS_THERMO_HI);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_HI_ONOFF && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_THERMO_HI);
    update_thermo(IS_THERMO_HI);
    return;
  } 
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_HI_ONOFF && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_THERMO_HI);
      update_thermo(IS_THERMO_HI);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_HI_ONOFF && ev == EV_KEY_SET_UP) {
    write_only(IS_THERMO_HI);
    lpress_start = 0;
    return;
  }
  
  // 调整thermo lo
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_MOD_PRESS) {
    enter_thermo(IS_THERMO_LO);
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_THERMO_LO);
    update_thermo(IS_THERMO_LO);
    return;
  }
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_THERMO_LO);
      update_thermo(IS_THERMO_LO);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_SET_UP) {
    write_only(IS_THERMO_LO);
    lpress_start = 0;
    return;
  }
  
  // 设置password
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_PASSWORD && ev == EV_KEY_MOD_PRESS) {
    if(get_sm_ss_state(from) == SM_FUSE_PARAM_PASSWORD) {
      if(common_state > 0) {
        common_state --;
      } else {
        set_task(EV_FUSE_SEL0);
        return;
      }
    }
    enter_password(IS_PASSWORD + common_state); // 使用last_display_s作为password的index，为了减少ram使用。。。
    return;
  }
  
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_PASSWORD && ev == EV_KEY_SET_PRESS) {
    inc_and_write(IS_PASSWORD + common_state);
    update_password(IS_PASSWORD + common_state);
    return;
  }
  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_PASSWORD + common_state);
      update_password(IS_PASSWORD + common_state);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    return;
  } 

  if(get_sm_ss_state(to) == SM_FUSE_PARAM_THERMO_LO_ONOFF && ev == EV_KEY_SET_UP) {
    write_only(IS_PASSWORD + common_state);
    lpress_start = 0;
    return;
  }
}