#include <STC89C5xRC.H>
#include <string.h>
#include "debug.h"
#include "i2c.h"
#include "rtc.h"
#include "task.h"

sbit RTC_RESET = P1 ^ 5;

#define RTC_RESET_PULSE_DELAY 20
#define RTC_I2C_ADDRESS  0xD0 //11010000

#define RTC_TIME_OFFSET 0x00
#define RTC_DATE_OFFSET 0x03
#define RTC_ALARM0_OFFSET 0x07
#define RTC_ALARM1_OFFSET 0x0B
#define RTC_CTL_OFFSET 0x0E
#define RTC_TEMP_OFFSET 0x11
#define RTC_USR0_OFFSET 0x14
#define RTC_USR1_OFFSET 0x08

#define RTC_CTL_OFFSET 0x0E

static void rtc_ISR (void) interrupt 2 using 1
{
  set_task(EV_ALARM0); // 这里是EV_ALARM0或者EV_ALARM1都可以
  IE1 = 0; // 清除中断标志位
}

static unsigned char rtc_data[4];
static unsigned char last_read;

static void dump_rtc(void)
{
	unsigned char addr;
	unsigned char c;
	// dump rtc contents
	for(addr = 0; addr < 0x15; addr ++) {
		I2C_Get(RTC_I2C_ADDRESS, addr, &c);
		CDBG("%bx = %bx\n", addr,  c);
	}
}

void rtc_initialize (void)
{
	unsigned char count = RTC_RESET_PULSE_DELAY;

	CDBG("rtc_initialize\n");
	
	I2C_Init();

  // reset rtc
	RTC_RESET = 0;
	while(count --);
	RTC_RESET = 1;
    
  memset(rtc_data, 0, sizeof(rtc_data));

  // 初始化
  // 初始时钟设置为 12小时格式，2014-08-19, 12:10：30 AM
  rtc_read_data(RTC_TYPE_TIME);
  CDBG("before time %bx %bx %bx %bx\n", rtc_data[0], rtc_data[1], rtc_data[2], rtc_data[3]);
  rtc_time_set_hour_12(1);
  rtc_time_set_hour(12);
  rtc_time_set_min(59);
  rtc_time_set_sec(30); 
  CDBG("after time %bx %bx %bx %bx\n", rtc_data[0], rtc_data[1], rtc_data[2], rtc_data[3]);  
  rtc_write_data(RTC_TYPE_TIME);
  
  rtc_read_data(RTC_TYPE_DATE);
  CDBG("before date %bx %bx %bx %bx\n", rtc_data[0], rtc_data[1], rtc_data[2], rtc_data[3]); 
  rtc_date_set_year(14);
  rtc_date_set_month(8);
  rtc_date_set_date(19);
  rtc_date_set_day(2);
  CDBG("after date %bx %bx %bx %bx\n", rtc_data[0], rtc_data[1], rtc_data[2], rtc_data[3]); 
  rtc_write_data(RTC_TYPE_DATE);

  // 闹钟0：一般闹钟，设置为12小时，12:00:00 AM
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_data[3] = 0x80; // A1M4 = 1
  rtc_data[2] = 0; // A1M3 = 0
  rtc_data[1] = 0; // A1M2 = 0
  rtc_data[0] = 0; // A1M1 = 0
  rtc_alarm_set_hour_12(1);
  rtc_alarm_set_hour(12);
  rtc_alarm_set_min(0);
  rtc_write_data(RTC_TYPE_ALARM0);  
  
  
  // 闹钟1：整点报时闹钟，设置为12小时，1:00：00 PM
  rtc_read_data(RTC_TYPE_ALARM1);
  rtc_data[2] = 0x80; // A2M3 = 1
  rtc_data[1] = 0;    // A2M2 = 0
  rtc_data[0] = 0;    // A2M1 = 0
  rtc_alarm_set_hour_12(1);
  rtc_alarm_set_hour(13);
  rtc_alarm_set_min(0);
  rtc_write_data(RTC_TYPE_ALARM1);  

  // 清除中断标志位
  I2C_Get(RTC_I2C_ADDRESS, 0x0F, &count);
  count &= ~0x3; // A1F,A2F = 00
  I2C_Put(RTC_I2C_ADDRESS, 0x0E, count);  

  // 允许RTC发中断
  I2C_Get(RTC_I2C_ADDRESS, 0x0E, &count);
  count |= 0x7; // INCN,A1E,A2E = 111
  I2C_Put(RTC_I2C_ADDRESS, 0x0E, count);
  
  
  IT1 = 1; // 设置为边沿触发
  EX1 = 1; // 开中断
  
  dump_rtc();
  
}


void rtc_read_data(enum rtc_data_type type)
{
  unsigned char offset;
  switch(type) {
    case RTC_TYPE_TIME:
      offset = RTC_TIME_OFFSET; break;
    case RTC_TYPE_DATE:
      offset = RTC_DATE_OFFSET; break;    
    case RTC_TYPE_ALARM0:
      offset = RTC_ALARM0_OFFSET; break;
    case RTC_TYPE_ALARM1:
      offset = RTC_ALARM1_OFFSET; break;
    case RTC_TYPE_TEMP:
      offset = RTC_TEMP_OFFSET; break; 
    case RTC_TYPE_CTL:
      offset = RTC_CTL_OFFSET; break;
    case RTC_TYPE_USR0:
      offset = RTC_USR0_OFFSET; break; 
    case RTC_TYPE_USR1:
      offset = RTC_USR1_OFFSET; break;     
  }
  
  last_read = type;
  
  I2C_Gets(RTC_I2C_ADDRESS, offset, sizeof(rtc_data), rtc_data);
  
}

void rtc_write_data(enum rtc_data_type type)
{
  unsigned char offset;
  switch(type) {
    case RTC_TYPE_TIME:
      offset = RTC_TIME_OFFSET; break;
    case RTC_TYPE_DATE:
      offset = RTC_DATE_OFFSET; break;    
    case RTC_TYPE_ALARM0:
      offset = RTC_ALARM0_OFFSET; break;
    case RTC_TYPE_ALARM1:
      offset = RTC_ALARM1_OFFSET; break;
    case RTC_TYPE_TEMP:
      offset = RTC_TEMP_OFFSET; break; 
    case RTC_TYPE_CTL:
      offset = RTC_CTL_OFFSET; break;    
    case RTC_TYPE_USR0:
      offset = RTC_USR0_OFFSET; break; 
    case RTC_TYPE_USR1:
      offset = RTC_USR1_OFFSET; break;     
  }
  I2C_Puts(RTC_I2C_ADDRESS, offset, sizeof(rtc_data), rtc_data);  
}

static unsigned char _rtc_get_hour(unsigned char hour)
{
  unsigned char ret;
  if(hour & 0x40) { // 是12小时表示
    ret = (hour & 0x0F) + ((hour & 0x10) >> 4) * 10;
    if( hour & 0x20 ) { // 是PM
      ret += 12;
    }
  } else { // 是24小时表示
      ret =  (hour & 0x0F) + ((hour & 0x30) >> 4) * 10;
  }
  return ret;
}


static void _rtc_set_hour(unsigned char hour, unsigned char * dat)
{
  if(*dat & 0x40) { // 是12小时表示
    if(hour > 12) {
      *dat |= 0x20; // 设置PM标志
      hour -= 12;
    } else {
      *dat &= ~0x20; // 清除PM标志
    }
    *dat &= 0xF0; // clear lsb
    *dat |= hour % 10;
    *dat &= 0xEF; // clear msb
    *dat |= (hour / 10) << 4;    
  } else {
    // 24小时表示
    *dat &= 0xF0; // clear lsb
    *dat |= hour % 10;
    *dat &= 0xCF; // clear msb
    *dat |= (hour / 10) << 4;
  }  
}

static void _rtc_set_hour_12(bit enable, unsigned char * dat)
{
  unsigned char hour;
  hour = _rtc_get_hour(* dat);
  if(enable) {
    *dat |= 0x40;
  } else {
    *dat &= ~0x40;
  }
  _rtc_set_hour(hour, dat);  
}

static bit _rtc_get_hour_12(unsigned char hour)
{
  return ((hour & 0x40) != 0);
}

static unsigned char _rtc_get_min_sec(unsigned char min)
{
  return (min & 0x0F) + ((min & 0x70) >> 4) * 10;
}

static void _rtc_set_min_sec(unsigned char min, unsigned char * dat)
{
  *dat &= 0xF0;
  *dat |= min % 10;
  *dat &= 0x8F;
  *dat |= (min / 10) << 4;
}

// 在read_rtc_data(RTC_TYPE_TIME)之后调用
// 此函数始终返回24小时格式的时间！
unsigned char rtc_time_get_hour(void)
{
  return _rtc_get_hour(rtc_data[2]);
}


// 此函数时钟传入24小时格式的时间
void rtc_time_set_hour(unsigned char hour)
{
  _rtc_set_hour(hour, &rtc_data[2]);
}

void rtc_time_set_hour_12(bit enable)
{
  _rtc_set_hour_12(enable, &rtc_data[2]);
}

bit rtc_time_get_hour_12()
{
  return _rtc_get_hour_12(rtc_data[2]);
}

unsigned char rtc_time_get_min(void)
{
  return _rtc_get_min_sec(rtc_data[1]);
}

void rtc_time_set_min(unsigned char min)
{
  _rtc_set_min_sec(min, &rtc_data[1]);
}

unsigned char rtc_time_get_sec(void)
{
  return _rtc_get_min_sec(rtc_data[0]);
}

void rtc_time_set_sec(unsigned char sec)
{
  _rtc_set_min_sec(sec, &rtc_data[0]);
}

// 在rtc_read_data(RTC_TYPE_DATE)之后调用
unsigned char rtc_date_get_year()
{
  return (rtc_data[3] & 0x0F) + ((rtc_data[3] & 0xF0) >> 4) * 10;
}

void rtc_date_set_year(unsigned char year)
{
  rtc_data[3] &= 0xF0;
  rtc_data[3] |= year % 10;
  rtc_data[3] &= 0x0F;
  rtc_data[3] |= (year / 10) << 4;  
}

unsigned char rtc_date_get_month()
{
  return (rtc_data[2] & 0x0F) + ((rtc_data[2] & 0x10) >> 4) * 10;
}

void rtc_date_set_month(unsigned char month)
{
  rtc_data[2] &= 0xF0;
  rtc_data[2] |= month % 10;
  rtc_data[2] &= 0x0F;
  rtc_data[2] |= (month / 10) << 4;  
}

unsigned char rtc_date_get_date()
{
  return (rtc_data[1] & 0x0F) + ((rtc_data[1] & 0xF0) >> 4) * 10;
}

unsigned char rtc_yymmdd_to_day(unsigned char year, unsigned char mon, unsigned char date)
{
  // 2000-1-1 is 6 (saturday)
  unsigned int d,m,y;
  d = date;
  m = mon;
  y = 2000 + year;
  return (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) % 7 + 1;
}

// 0 ~ 99
bit rtc_is_leap_year(unsigned char y)
{
  int year = 2000 + y;
  if((year % 100) != 0 && (year % 4) == 0
  || (year % 400) == 0) {
    return 1;
  } else {
    return 0;
  }
}

// 此函数需要检查合法性！！
bit rtc_date_set_date(unsigned char date)
{
  char mon = rtc_date_get_month();
  
  CDBG("rtc_date_set_day, valid check...\n");
  if(mon == 1 || mon == 3 || mon == 5 || mon == 7 
    || mon == 8 || mon == 10 || mon == 12) {
    if(date > 32) return 1;
  } else {
    if(date > 31) return 1;
    if(mon == 2 && rtc_is_leap_year(rtc_date_get_year())) {
      if(date > 30) return 1;
    } else if(mon == 2 && !rtc_is_leap_year(rtc_date_get_year())) {
      if(date > 29) return 1;
    }
  }
  
  CDBG("rtc_date_set_day, valid check...OK\n");
  
  rtc_data[1] &= 0xF0;
  rtc_data[1] |= date % 10;
  rtc_data[1] &= 0x0F;
  rtc_data[1] |= (date / 10) << 4; 

  return 0;  
}

unsigned char rtc_date_get_day()
{
  return rtc_data[0];
}

void rtc_date_set_day(unsigned char day)
{
  if (day >= 1 && day <= 7) {
    rtc_data[0] = day;
  }
}

// 在rtc_read_data(RTC_TYPE_ALARM0)或者RTC_TYPE_ALARM1之后调用
unsigned char rtc_alarm_get_hour()
{
  return _rtc_get_hour(last_read == RTC_TYPE_ALARM0 ? rtc_data[2]:rtc_data[1]);
}

bit rtc_alarm_get_hour_12()
{
  return _rtc_get_hour_12(last_read == RTC_TYPE_ALARM0 ? rtc_data[2] : rtc_data[1]);
}

void rtc_alarm_set_hour_12(bit enable)
{
  _rtc_set_hour_12(enable, last_read == RTC_TYPE_ALARM0 ? &rtc_data[2] : &rtc_data[1]);
}

void rtc_alarm_set_hour(unsigned char hour)
{
  _rtc_set_hour(hour, last_read == RTC_TYPE_ALARM0 ? &rtc_data[2] : &rtc_data[1]);
}

unsigned char rtc_alarm_get_min()
{
  return _rtc_get_min_sec(last_read == RTC_TYPE_ALARM0 ? rtc_data[1] : rtc_data[0]);
}

void rtc_alarm_set_min( unsigned char min)
{
  _rtc_set_min_sec(min, last_read == RTC_TYPE_ALARM0 ? &rtc_data[1] : &rtc_data[0]); 
}
// 在rtc_read_data(RTC_TYPE_TEMP)之后调用
bit rtc_get_temperature(unsigned char * integer, unsigned char * flt)
{
  float ret = 0;
  bit sign = ((rtc_data[0] &  0x80) != 0);

  
  if(sign) { // 是负数
    rtc_data[0] = ~rtc_data[0];
    rtc_data[1] &= 0xC0;
    rtc_data[1] >>= 6;
    rtc_data[1] = ~rtc_data[1] + 1;
    rtc_data[1] &= 0x03;
    if(rtc_data[1] == 0) {
      rtc_data[0] ++;
    }
  } else { //是正数
    rtc_data[1] &= 0xC0;
    rtc_data[1] >>= 6;
  }
  
  *integer = rtc_data[0];
  
  if((rtc_data[1] & 0x3) == 0x3) {
    *flt = 75;
  } else if((rtc_data[1] & 0x3) == 0x2) { 
    *flt = 50;
  } else if((rtc_data[1] & 0x3) == 0x1) { 
    *flt = 25;
  } else {
    *flt = 0;
  }

  if(*integer > 99) *integer = 99;
 
  return sign;
}

// 在rtc_read_data（RTC_TYPE_CTL）之后调用
void rtc_enable_alarm_int(bit enable, unsigned char index)
{
  if(index == 0) {
    if(!enable)
      rtc_data[0] &= ~1;
    else
      rtc_data[0] |= 1;
  } else if(index == 1) {
    if(!enable)
      rtc_data[0] &= ~2;
    else
      rtc_data[0] |= 2;
  }
}

bit rtc_test_alarm_int(unsigned char index)
{
  if(index == 0) {
    return (rtc_data[0] & 1) == 1;
  } else if(index == 1) {
    return (rtc_data[0] & 2) == 2;
  }
  return 0;
}

void rtc_clr_alarm_int_flag(unsigned char index)
{
  if(index == 0) {
    rtc_data[1] &= ~1;
  } else if(index == 1) {
    rtc_data[1] &= ~2;    
  } 
}

bit rtc_test_alarm_int_flag(unsigned char index)
{
  if(index == 0) {
    return (rtc_data[1] & 1) == 1;
  } else if(index == 1) {
    return (rtc_data[1] & 2) == 2;
  }
  return 0;
}

void rtc_enter_powersave(void)
{
  
}

void rtc_leave_powersave(void)
{
  
}