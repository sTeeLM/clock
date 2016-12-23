#include <STC89C5xRC.H>
#include <stdio.h>

#include "task.h"
#include "debug.h"
#include "cext.h"
#include "led.h"
#include "misc.h"
#include "clock.h"
#include "rtc.h"

// ISR里不能调带参数函数。。。
// 2000~2099年
static unsigned char code date_table[100][12] = 
{
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
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,29,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,},
{31,28,31,30,31,30,31,31,30,31,30,31,}
};

static struct clock idata clk;
static bit clk_is12;

#pragma NOAREGS
static void clock_inc_ms39(void)
{
  clk.ms39 ++;
  if((clk.ms39 % 2 ) == 0) {
     refresh_led();
  }
  
  if((clk.ms39 % 6 ) == 0) {
     set_task(EV_SCAN_KEY); 
  }
  
  if((clk.ms39 % 64) == 0) {
    set_task(EV_250MS);
  }
    
  if(clk.ms39 == 0 ) {
    clk.sec = ( ++clk.sec) % 60;
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
}
#pragma AREGS 

void clock_dump(void)
{
  CDBG("clk.year = %bd\n", clk.year);
  CDBG("clk.mon  = %bd\n", clk.mon);
  CDBG("clk.date = %bd\n", clk.date); 
  CDBG("clk.day  = %bd\n", clk.day);
  CDBG("clk.hour = %bd\n", clk.hour);
  CDBG("clk.min  = %bd\n", clk.min);
  CDBG("clk.sec  = %bd\n", clk.sec);  
  CDBG("clk.ms39 = %bu\n", clk.ms39);
  CDBG("clk.is12 = %s\n", clk_is12 ? "ON" : "OFF"); 
}

bit clock_get_hour_12(void)
{
  return clk_is12;
}
void clock_set_hour_12(bit enable)
{
  clk_is12 = enable;
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
  clk.date = (clk.date ++) % clock_get_mon_date(clk.year, clk.mon);
  clk.day = (++ clk.day) % 7;
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
}

unsigned char clock_get_year(void)
{
  return clk.year;
}
void clock_inc_year(void)
{
  clk.year = (++ clk.year) % 100;
}

void clock_sync_from_rtc(enum clock_sync_type type)
{
  CDBG("clock_sync_from_rtc = %bd\n", type);
  if(type == CLOCK_SYNC_TIME) {
    rtc_read_data(RTC_TYPE_TIME);
    clk.hour = rtc_time_get_hour();   // 0 - 23
    clk.min  = rtc_time_get_min();    // 0 - 59
    clk.sec  = rtc_time_get_sec();    // 0 - 59
    clk.ms39 = 0;   // 0 - 255
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
  CDBG("clock_sync_to_rtc = %bd\n", type);
  if(type == CLOCK_SYNC_TIME) {
    rtc_read_data(RTC_TYPE_TIME);
    rtc_time_set_hour(clk.hour);
    rtc_time_set_min(clk.min);
    rtc_time_set_sec(clk.sec);
    rtc_time_set_hour_12(clk_is12);
    rtc_write_data(RTC_TYPE_TIME);
  } else if(type == CLOCK_SYNC_DATE) {
    rtc_read_data(RTC_TYPE_DATE);
    rtc_date_set_year(clk.year);             // 0 - 99 (2000 ~ 2099)
    rtc_date_set_month(clk.mon + 1);         // 0 - 11
    rtc_date_set_date(clk.date + 1);         // 0 - 30(29/28/27)
    rtc_write_data(RTC_TYPE_DATE);
  }
}

static void clock0_ISR (void) interrupt 1 using 1
{
  clock_inc_ms39();
  TF0 = 0;
}

/*
static void clock0_ISR (void) interrupt 1 using 1 
{
   counter_3p9ms ++;
   if((counter_1ms % 2 ) == 0) {
     refresh_led();
   }
 
   if((counter_1ms % 25) == 0) {
     set_task(EV_SCAN_KEY); 
     counter_25ms ++;
     if((counter_25ms % 10) == 0) {
       set_task(EV_250MS); 
       counter_250ms ++;
       if((counter_250ms % 4) == 0) {
         counter_1s ++;
         set_task(EV_1S); 
       }
     }
   }
   TF0 = 0;
}
*/


// 辅助函数
bit clock_is_leap_year(unsigned char year)
{
  if(year >= 99) year = 99;
  return date_table[year][1] == 29;
}

unsigned char clock_get_mon_date(unsigned char year, unsigned char mon)
{
  if(year >= 99) year = 99;
  if(mon >= 12) mon = 11;
  return date_table[year][mon];
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
  ET0 = 1; // 开中断
  TR0 = 1; //开始了
  clock_dump();
}

/*
void clock_initialize (float fclk)
{

   int clkr_reload;
  
	 CDBG("clock_initialize %f\n", fclk);  
  
   counter_1ms = 0;  // initialize state counter
	 counter_25ms = 0;
   counter_250ms = 0;
   counter_1s = 0;
  
	// initialization T2CON:
   //   CP/RL2 = 0 (autoreload, no capture),
   //   EXEN2 = 0 (disable external input T2EX),
   //   C/T2 = 0 (clock, no counter),
   //   RCLK = TCLK = 0 (clock, no baud rate generator),
   //   TF2 = EXF2 = 0 (interrupt flags cleared) 
   //   TR2 = 0 (clock stop, please)
   T2CON = 4;

   // Load clock2 autoreload bytes
   clkr_reload = (0xFFFF - (unsigned int)((fclk * TMR_TIME) / 12));
   RCAP2H = high_byte(clkr_reload); 
   RCAP2L = low_byte(clkr_reload);

   // Highest priority for Timer2 interrupts
   PT2 = 1;
   // Enables Timer2 interrupts
   ET2 = 1;
   // Start Timer2
   TR2 = 1;	
}
*/

void clock_enter_powersave(void)
{
  ET0 = 0;
}

void clock_leave_powersave(void)
{
  ET0 = 1;
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE); 
  clock_sync_from_rtc(CLOCK_SYNC_TIME);
  clock_sync_from_rtc(CLOCK_SYNC_DATE);   
}

