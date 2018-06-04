#include <STC89C5xRC.H>
#include <stdio.h>

#include "task.h"
#include "debug.h"
#include "cext.h"
#include "led.h"
#include "misc.h"
#include "clock.h"
#include "rtc.h"
#include "timer.h"
#include "lt_timer.h"

// ISR里不能调带参数函数。。。
// 2000~2099年
static unsigned char code date_table[100][12] = 
{
{31,29,31,30,31,30,31,31,30,31,30,31,}, // 2000
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,}
};

static struct clock_struct idata clk;
static bit clk_is12;
static unsigned char idata sec_256; // 用于 time_diff

static bit display_enable;
static unsigned char display_mode;

static bit in_shell;

void clock_display(bit enable)
{
  display_enable = enable;
}

void clock_switch_display_mode(enum clock_display_mode mode)
{
  display_mode = mode;
}

#pragma NOAREGS
static void clock_inc_ms39(void)
{
  unsigned char hour;
  bit is_pm;
  
  clk.ms39 ++;
  
  if((clk.ms39 % 6 ) == 0) {
     set_task(EV_SCAN_KEY);
  } 
  
  if((clk.ms39 % 64) == 0) {
    set_task(EV_250MS);
  }
    
  if(clk.ms39 == 0 ) {
    clk.sec = (++ clk.sec) % 60;
    lt_timer_dec_sec();
    sec_256 ++;
    set_task(EV_1S);
    if(clk.sec == 0) {
      clk.min = (++ clk.min) % 60;
      if(clk.min == 0) {
        clk.hour = (++ clk.hour) % 24;
        if(clk.hour == 0) {
          clk.date = (++ clk.date) % date_table[clk.year][clk.mon];
          clk.day = (++ clk.day) % 7;
          if(clk.day == 0) {
            clk.mon = (++ clk.mon) % 12;
            if(clk.mon == 0) {
              clk.year = (++ clk.year) % 100;
            }
          }
        }
      }
    } 
  }
  if(display_enable && !led_powersave) {
    if(display_mode == CLOCK_DISPLAY_MODE_HHMMSS) {
      hour = clk.hour;
      is_pm = 0;
      if(clk_is12 && clk.hour > 12) {
        hour -= 12;
        is_pm = 1;
      } else if(clk_is12 && clk.hour == 12) { // 中午12点是PM。。
				is_pm = 1;
			}
      
      if(hour / 10 != 0) {
        led_data[5] = led_code[hour / 10 + 4] | (is_pm ? 0 : 0x80);
      } else {
        led_data[5] = 0x7F | (is_pm ? 0 : 0x80);
      }
      led_data[4] = led_code[hour % 10 + 4];
      led_data[3] = led_code[clk.min / 10 + 4];
      led_data[2] = led_code[clk.min % 10 + 4];
      led_data[1] = led_code[clk.sec / 10 + 4]; 
      led_data[0] = led_code[clk.sec % 10 + 4]|0x80;
    } else if(display_mode == CLOCK_DISPLAY_MODE_YYMMDD) {
      led_data[5] = led_code[clk.year / 10 + 4] |0x80;
      led_data[4] = led_code[clk.year % 10 + 4];
      led_data[3] = led_code[(clk.mon + 1) / 10 + 4] |0x80;
      led_data[2] = led_code[(clk.mon + 1) % 10 + 4];
      led_data[1] = led_code[(clk.date + 1) / 10 + 4] |0x80; 
      led_data[0] = led_code[(clk.date + 1) % 10 + 4]|0x80;
    } else {
      led_data[5] = led_code[0x18]|0x80; // D
      led_data[4] = led_code[0x15]|0x80; // A
      led_data[3] = led_code[0x2D]|0x80; // Y
      led_data[2] = led_code[0x01]|0x80; // -
      led_data[1] = led_code[(clk.day + 1) / 10 + 4] |0x80; 
      led_data[0] = led_code[(clk.day + 1) % 10 + 4] |0x80;
    }
  }
}
#pragma AREGS 

void clock_dump(void)
{
  CDBG("clk.year = %bu\n", clk.year);
  CDBG("clk.mon  = %bu\n", clk.mon);
  CDBG("clk.date = %bu\n", clk.date); 
  CDBG("clk.day  = %bu\n", clk.day);
  CDBG("clk.hour = %bu\n", clk.hour);
  CDBG("clk.min  = %bu\n", clk.min);
  CDBG("clk.sec  = %bu\n", clk.sec);  
  CDBG("clk.ms39 = %bu\n", clk.ms39);
  CDBG("clk.is12 = %s\n", clk_is12 ? "ON" : "OFF"); 
}

// 计算某年某月某日星期几,  经典的Zeller公式
// year 0-99
// mon 0-11
// date 0-30
// return 0-6, 0 is monday, 6 is sunday
unsigned char clock_yymmdd_to_day(unsigned char year, unsigned char mon, unsigned char date)
{
  unsigned int d,m,y,c;
  d = date + 1;
  m = mon + 1;
  y = CLOCK_YEAR_BASE + year;
  
  if(m < 3){
    y -= 1;
    m += 12;
  }
  
  c = (unsigned int)(y / 100);
  y = y - 100 * c;
  
  c = (unsigned int)(c / 4) - 2 * c + y + (unsigned int) ( y / 4 ) + (26 * (m + 1) / 10) + d - 1;
  c = (c % 7 + 7) % 7;

  if(c == 0) {
    return 6;
  }
  
  return c - 1;
}

bit clock_get_hour_12(void)
{
  return clk_is12;
}
void clock_set_hour_12(bit enable)
{
  clk_is12 = enable;
}

unsigned char clock_get_ms39(void)
{
  return clk.ms39;
}

unsigned char clock_get_sec_256(void)
{
  return sec_256;
}

unsigned char clock_get_sec(void)
{
  return clk.sec;
}
void clock_clr_sec(void)
{
   clk.sec = 0;
}

unsigned char clock_get_min(void)
{
  return clk.min;
}
void clock_inc_min(void)
{
  clk.min = (++ clk.min) % 60;
}

unsigned char clock_get_hour(void)
{
  return clk.hour;
}
void clock_inc_hour(void)
{
  clk.hour = (++ clk.hour) % 24;
}


unsigned char clock_get_date(void)
{
  return clk.date + 1;
}
void clock_inc_date(void)
{
  clk.date = ( ++ clk.date) % clock_get_mon_date(clk.year, clk.mon);
  clk.day = clock_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

unsigned char clock_get_day(void)
{
  return clk.day + 1;
}

unsigned char clock_get_month(void)
{
  return clk.mon + 1;
}
void clock_inc_month(void)
{
  clk.mon = (++ clk.mon) % 12;
  clk.day = clock_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

unsigned char clock_get_year(void)
{
  return clk.year;
}
void clock_inc_year(void)
{
  clk.year = (++ clk.year) % 100;
  clk.day = clock_yymmdd_to_day(clk.year, clk.mon, clk.date);
}

void clock_sync_from_rtc(enum clock_sync_type type)
{
  CDBG("clock_sync_from_rtc = %bu\n", type);
  if(type == CLOCK_SYNC_TIME) {
    rtc_read_data(RTC_TYPE_TIME);
    clk.hour = rtc_time_get_hour();   // 0 - 23
    clk.min  = rtc_time_get_min();    // 0 - 59
    clk.sec  = rtc_time_get_sec();    // 0 - 59
    clk.ms39 = 255;   // 0 - 255
    clk_is12     = rtc_time_get_hour_12();
  } else if(type == CLOCK_SYNC_DATE) {
    rtc_read_data(RTC_TYPE_DATE);
    clk.year = rtc_date_get_year();          // 0 - 99 (2000 ~ 2099)
    clk.mon  = rtc_date_get_month() - 1;     // 0 - 11
    clk.date = rtc_date_get_date() - 1;      // 0 - 30(29/28/27)
    clk.day  = rtc_date_get_day() - 1;       // 0 - 6
  }
}

void clock_sync_to_rtc(enum clock_sync_type type)
{
  CDBG("clock_sync_to_rtc = %bu\n", type);
  clock_enable_interrupt(0);
  if(type == CLOCK_SYNC_TIME) {
    rtc_read_data(RTC_TYPE_TIME);
    rtc_time_set_hour_12(clk_is12);
    rtc_time_set_hour(clk.hour);
    rtc_time_set_min(clk.min);
    rtc_time_set_sec(clk.sec);
    rtc_write_data(RTC_TYPE_TIME);
  } else if(type == CLOCK_SYNC_DATE) {
    rtc_read_data(RTC_TYPE_DATE);
    rtc_date_set_year(clk.year);             // 0 - 99 (2000 ~ 2099)
    rtc_date_set_month(clk.mon + 1);         // 0 - 11
    rtc_date_set_date(clk.date + 1);         // 0 - 30(29/28/27)
    rtc_write_data(RTC_TYPE_DATE);
  }
  clock_enable_interrupt(1);
}

static void clock0_ISR (void) interrupt 1 using 1
{  

  refresh_led();	
	
  if(in_shell) {
    TF0 = 0;
    return;
  }
  
  clock_inc_ms39();
  timer_inc_ms39();
	
	TF0 = 0;
}


// 辅助函数
bit clock_is_leap_year(unsigned char year)
{
  if(year >= 100) year = 99;
  return date_table[year][1] == 29;
}

unsigned char clock_get_mon_date(unsigned char year, unsigned char mon)
{
  if(year >= 100) year = 99;
  if(mon >= 12) mon = 11;
  return date_table[year][mon];
}

void clock_enable_interrupt(bit enable)
{
  ET0 = enable;
  TR0 = enable;
}

void clock_initialize(void)
{
  CDBG("clock_initialize\n");
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE); 
  // GATE = 0
  // CT = 1
  // M1 = 1
  // M2 = 0
  TMOD |= 0x06; // 工作在模式2
  TL0 = (256 - 128); // 32768HZ方波输入，3.90625ms中断一次（256个中断是1s）
  TH0 = (256 - 128);
  PT0 = 1; // 最高优先级 
  display_mode = CLOCK_DISPLAY_MODE_HHMMSS;
  display_enable = 0;
  
  clock_enable_interrupt(1);

  clock_dump();
}


void clock_enter_powersave(void)
{
  CDBG("clock_enter_powersave\n");
  clock_enable_interrupt(0);
}

void clock_leave_powersave(void)
{
  CDBG("clock_leave_powersave\n");
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(1);
}

void clock_enter_shell(void)
{
  in_shell = 1;
}

void clock_leave_shell(void)
{
  in_shell = 0;
  clock_enable_interrupt(0);
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);
  clock_enable_interrupt(1);
}
