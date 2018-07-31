#include <STC89C5xRC.H>
#include <string.h>
#include "debug.h"
#include "i2c.h"
#include "rtc.h"
#include "clock.h"
#include "task.h"
#include "alarm.h"
#include "lt_timer.h"
#include "serial_hub.h"
#include "rom.h"
#include "misc.h"

#define RTC_I2C_ADDRESS  0xD0 //11010000

#define RTC_TIME_OFFSET 0x00
#define RTC_DATE_OFFSET 0x03
#define RTC_ALARM0_OFFSET 0x07
#define RTC_ALARM1_OFFSET 0x0B
#define RTC_CTL_OFFSET 0x0E
#define RTC_TEMP_OFFSET 0x11

#define RTC_CTL_OFFSET 0x0E

static unsigned char rtc_data[4];
static unsigned char last_read;
static bit is_lt_timer_mode;

#ifdef __CLOCK_DEBUG__
static const char * code rtc_alarm_mode_str[] = 
{
  "RTC_ALARM0_MOD_PER_SEC",
  "RTC_ALARM0_MOD_MATCH_SEC",  
  "RTC_ALARM0_MOD_MATCH_MIN_SEC", 
  "RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC", 
  "RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC",   
  "RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC",
  "RTC_ALARM0_MOD_CNT", 
  "RTC_ALARM1_MOD_PER_MIN",
  "RTC_ALARM1_MOD_MATCH_MIN",  
  "RTC_ALARM1_MOD_MATCH_HOUR_MIN", 
  "RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN",   
  "RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN", 
};

static const char * code rtc_square_rate_str[] = 
{
  "RTC_SQUARE_RATE_1HZ",
  "RTC_SQUARE_RATE_1024HZ",
  "RTC_SQUARE_RATE_4096HZ",
  "RTC_SQUARE_RATE_8192HZ"
};

#endif

static void rtc_dump_raw(void)
{
  unsigned char addr;
  unsigned char c;
  CDBG(("RTC raw content:\n"));
  for(addr = 0; addr < 0x15; addr ++) {
    I2C_Get(RTC_I2C_ADDRESS, addr, &c);
    CDBG(("rtc [%02bx] = 0x%02bx\n", addr,  c));
  }
}

void rtc_dump(void)
{
  CDBG(("rtc_dump:\n"));
  rtc_read_data(RTC_TYPE_DATE);
  CDBG(("date/day: %02bu-%02bu-%02bu/%bu\n",
    rtc_date_get_year(), rtc_date_get_month(), rtc_date_get_date(),
    rtc_date_get_day()
  ));
  
  rtc_read_data(RTC_TYPE_TIME);
  CDBG(("time: %02bu:%02bu:%02bu, is12: %s\n",
    rtc_time_get_hour(), rtc_time_get_min(), rtc_time_get_sec(),
    rtc_time_get_hour_12() ? "ON" : "OFF"
  ));
  
  rtc_read_data(RTC_TYPE_ALARM0);
  CDBG(("alarm0 mode: %s\n", rtc_alarm_get_mod_str()));
  CDBG(("  day:%02bu\n", rtc_alarm_get_day()));
  CDBG(("  date:%02bu\n", rtc_alarm_get_date()));  
  CDBG(("  hour:%02bu\n", rtc_alarm_get_hour()));
  CDBG(("  min:%02bu\n", rtc_alarm_get_min()));  
  CDBG(("  sec:%02bu\n", rtc_alarm_get_sec()));
  CDBG(("  is12:%s\n", rtc_alarm_get_hour_12() ? "ON" : "OFF"));  

  rtc_read_data(RTC_TYPE_ALARM1);
  CDBG(("alarm1 mode: %s\n", rtc_alarm_get_mod_str()));
  CDBG(("  day:%02bu\n", rtc_alarm_get_day()));
  CDBG(("  date:%02bu\n", rtc_alarm_get_date()));  
  CDBG(("  hour:%02bu\n", rtc_alarm_get_hour()));
  CDBG(("  min:%02bu\n", rtc_alarm_get_min()));  
  CDBG(("  is12:%s\n", rtc_alarm_get_hour_12() ? "ON" : "OFF"));
  
  rtc_read_data(RTC_TYPE_CTL);
  CDBG(("control:\n"));
  CDBG(("  alarm0 int enable:%s\n", rtc_test_alarm_int(RTC_ALARM0) ? "ON" : "OFF"));
  CDBG(("  alarm1 int enable:%s\n", rtc_test_alarm_int(RTC_ALARM1) ? "ON" : "OFF"));
  CDBG(("  alarm0 int flag:%s\n", rtc_test_alarm_int_flag(RTC_ALARM0) ? "ON" : "OFF"));
  CDBG(("  alarm1 int flag:%s\n", rtc_test_alarm_int_flag(RTC_ALARM1) ? "ON" : "OFF"));
  CDBG(("  eosc:%c\n", rtc_test_eosc() ? '1' : '0'));  
  CDBG(("  bbsqw:%c\n", rtc_test_bbsqw() ? '1' : '0'));  
  CDBG(("  conv:%c\n", rtc_test_conv() ? '1' : '0'));  
  CDBG(("  square_rate:%s\n", rtc_get_square_rate_str()));
  CDBG(("  intcn:%c\n", rtc_test_intcn() ? '1' : '0'));  
  CDBG(("  osf:%c\n", rtc_test_osf() ? '1' : '0'));  
  CDBG(("  en32khz:%c\n", rtc_test_en32khz() ? '1' : '0'));  
  CDBG(("  bsy:%c\n", rtc_test_bsy() ? '1' : '0'));  
}

bit rtc_is_lt_timer(void)
{
  return is_lt_timer_mode;
}

void rtc_set_lt_timer(bit enable)
{
  is_lt_timer_mode = enable;
}

void scan_rtc(void)
{   
  if(!is_lt_timer_mode) { // 正常闹钟模式
    scan_alarm();
  } else { // 长期定时器模式
    scan_lt_timer();
  }
}

void rtc_initialize (void)
{
  unsigned char count;

  CDBG(("RTC before initialize:\n"));
  rtc_dump_raw();
  rtc_dump();
  
  is_lt_timer_mode = 0;
    
  memset(rtc_data, 0, sizeof(rtc_data));

  // 初始化

  rtc_read_data(RTC_TYPE_TIME);

  // 12/24格式按照rom设置来，需要转换一次
  // RTC 内部使用24小时制
  count = rtc_time_get_hour();
  rtc_time_set_hour_12(0);
  rtc_time_set_hour(count);
  
  ///// 调试用，2014-08-19, 12:10:30 PM
  // rtc_time_set_hour(12);
  // rtc_time_set_min(10);
  // rtc_time_set_sec(30); 
  /////
  
  if(rom_is_factory_reset()) { //2014-08-19, 12:10:30 PM
    rtc_time_set_hour(12);
    rtc_time_set_min(10);
    rtc_time_set_sec(30); 
  }
  rtc_write_data(RTC_TYPE_TIME);
  
  delay_ms(10);
  
  rtc_read_data(RTC_TYPE_DATE);
 
  ///// 调试用，初始时钟设置为 12小时格式，2014-08-19, 12:10：30 AM
  // rtc_date_set_year(14);
  // rtc_date_set_month(8);
  // rtc_date_set_date(19);
  /////
  
  if(rom_is_factory_reset()) { // 2000-1-1
    rtc_date_set_year(14);
    rtc_date_set_month(8);
    rtc_date_set_date(19);
  }
  
  rtc_date_set_day(clock_yymmdd_to_day(
    rtc_date_get_year() ,
    rtc_date_get_month() - 1,
    rtc_date_get_date() - 1) + 1);
  
  rtc_write_data(RTC_TYPE_DATE);
  
  delay_ms(10);
  
  // 闹钟也设置为24小时格式
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_hour_12(0);
  rtc_write_data(RTC_TYPE_ALARM0);  
  
  rtc_read_data(RTC_TYPE_ALARM1);
  rtc_alarm_set_hour_12(0);
  rtc_write_data(RTC_TYPE_ALARM1);

  
  // 清除所有闹钟：闹钟配置由alarm自行从rom中读取，写入rtc
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, 0);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
  rtc_enable_alarm_int(RTC_ALARM1, 0);
  rtc_clr_alarm_int_flag(RTC_ALARM1);
  // 允许RTC发中断
  rtc_set_intcn(1);
  // 启动32KHZ输出  
  rtc_set_en32khz(1);
  // 清除eosc
  rtc_set_eosc(0);
  // BBSQW = 0，电池供电时候没有方波或者中断
  rtc_set_bbsqw(0);
  rtc_write_data(RTC_TYPE_CTL); 
  
  CDBG(("RTC after initialize:\n"));
  rtc_dump_raw();
  rtc_dump();
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

static unsigned char _rtc_get_date(unsigned char date)
{
  return (date & 0x0F) + ((date & 0x30) >> 4) * 10;
}

static void _rtc_set_date(unsigned char date, unsigned char * dat)
{
  *dat &= 0xF0;
  *dat |= date % 10;
  *dat &= 0xCF;
  *dat |= (date / 10) << 4; 
}

unsigned char rtc_date_get_date()
{
  return _rtc_get_date(rtc_data[1]);
}

// 此函数需要检查合法性！！
bit rtc_date_set_date(unsigned char date)
{
  char mon = rtc_date_get_month();
  
  CDBG(("rtc_date_set_day, valid check...\n"));
  if(mon == 1 || mon == 3 || mon == 5 || mon == 7 
    || mon == 8 || mon == 10 || mon == 12) {
    if(date > 32) return 1;
  } else {
    if(date > 31) return 1;
    if(mon == 2 && clock_is_leap_year(rtc_date_get_year())) {
      if(date > 30) return 1;
    } else if(mon == 2 && !clock_is_leap_year(rtc_date_get_year())) {
      if(date > 29) return 1;
    }
  }
  
  CDBG(("rtc_date_set_day, valid check...OK\n"));
  _rtc_set_date(date, &rtc_data[1]);

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

unsigned char rtc_alarm_get_day(void)
{
  return last_read == RTC_TYPE_ALARM0 ? (rtc_data[3] & 0x0F) : (rtc_data[2] & 0x0F);
}

void rtc_alarm_set_day(unsigned char day)
{
  if(last_read == RTC_TYPE_ALARM0) {
    rtc_data[3] &= 0xF0;
    rtc_data[3] |= day;
  } else {
    rtc_data[2] &= 0xF0;
    rtc_data[2] |= day;
  }
}

unsigned char rtc_alarm_get_date(void)
{
  return last_read == RTC_TYPE_ALARM0 ?
    _rtc_get_date(rtc_data[3]) : _rtc_get_date(rtc_data[2]);
}

void rtc_alarm_set_date(unsigned char date)
{
  last_read == RTC_TYPE_ALARM0 ? _rtc_set_date(date, &rtc_data[3]) : 
    _rtc_set_date(date, &rtc_data[2]);
}

unsigned char rtc_alarm_get_sec(void)
{
  if(last_read == RTC_TYPE_ALARM0) {
    return (rtc_data[0] & 0x0F) + ((rtc_data[0] & 0x70) >> 4) * 10;
  }
  return 0;
}


void rtc_alarm_set_sec( unsigned char sec)
{
  if(last_read == RTC_TYPE_ALARM0) {
    rtc_data[0] &= 0xF0;
    rtc_data[0] |= sec % 10;
    rtc_data[0] &= 0x8F;
    rtc_data[0] |= ((sec / 10) << 4);    
  }
}
// DY A1M4 A1M3 A1M2 A1M1
// X  1    1    1    1    ALARM0_MOD_PER_SEC                 Alarm once per second
// X  1    1    1    0    ALARM0_MOD_MATCH_SEC               Alarm when seconds match
// X  1    1    0    0    ALARM0_MOD_MATCH_MIN_SEC           Alarm when minutes and seconds match
// X  1    0    0    0    ALARM0_MOD_MATCH_HOUR_MIN_SEC      Alarm when hours, minutes, and seconds match
// 0  0    0    0    0    ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC Alarm when date, hours, minutes, and seconds match
// 1  0    0    0    0    ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  Alarm when day, hours, minutes, and seconds match

// DY A2M4 A2M3 A1M2
// X  1    1    1    ALARM1_MOD_PER_MIN                 Alarm once per minute (00 seconds of every minute)
// X  1    1    0    ALARM1_MOD_MATCH_MIN               Alarm when minutes match
// X  1    0    0    ALARM1_MOD_MATCH_HOUR_MIN          Alarm when hours and minutes match
// 0  0    0    0    ALARM1_MOD_MATCH_DATE_HOUR_MIN     Alarm when date, hours, and minutes match
// 1  0    0    0    ALARM1_MOD_MATCH_DAY_HOUR_MIN      Alarm when day, hours, and minutes match

enum rtc_alarm_mode rtc_alarm_get_mode(void)
{
  unsigned char dy, a1m1, a1m2, a1m3, a1m4;
  if(last_read == RTC_TYPE_ALARM0) {
    dy = rtc_data[3] & 0x40;
    a1m1 = rtc_data[0] & 0x80;
    a1m2 = rtc_data[1] & 0x80;
    a1m3 = rtc_data[2] & 0x80;
    a1m4 = rtc_data[3] & 0x80;
    if(a1m1 && a1m2 && a1m3 && a1m4) {
      return RTC_ALARM0_MOD_PER_SEC;
    } else if(!a1m1 && a1m2 && a1m3 && a1m4) {
      return RTC_ALARM0_MOD_MATCH_SEC;
    } else if(!a1m1 && !a1m2 && a1m3  && a1m4) {
      return RTC_ALARM0_MOD_MATCH_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && a1m4) {
      return RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC;
    } else if(!a1m1 && !a1m2 && !a1m3  && !a1m4 && !dy) {
      return RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC;
    } else {
      return RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC;
    }
  } else if(last_read == RTC_TYPE_ALARM1){
    dy = rtc_data[2] & 0x40;
    a1m2 = rtc_data[0] & 0x80;
    a1m3 = rtc_data[1] & 0x80;
    a1m4 = rtc_data[2] & 0x80;
    if(a1m2 && a1m3 && a1m4) {
      return RTC_ALARM1_MOD_PER_MIN;
    } else if(!a1m2 && a1m3 && a1m4) {
      return RTC_ALARM1_MOD_MATCH_MIN;
    } else if(!a1m2 && !a1m3  && a1m4) {
      return RTC_ALARM1_MOD_MATCH_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4) {
      return RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else if(!a1m2 && !a1m3  && !a1m4 && !dy) {
      return RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN;
    } else {
      return RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN;
    }
  }
  return RTC_ALARM0_MOD_CNT;
}
void rtc_alarm_set_mode(enum rtc_alarm_mode mode)
{
  if(mode < RTC_ALARM0_MOD_CNT && last_read == RTC_TYPE_ALARM0) {
    rtc_data[0] &= ~0x80;
    rtc_data[1] &= ~0x80;
    rtc_data[2] &= ~0x80;
    rtc_data[3] &= ~0x80; 
    rtc_data[3] &= ~0x40;    
    switch(mode) {
      case RTC_ALARM0_MOD_PER_SEC:
        rtc_data[0] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_SEC:
        rtc_data[1] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_MIN_SEC:
        rtc_data[2] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC:
        rtc_data[3] |=  0x80;
      case RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC:
        break;
      case RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC:
        rtc_data[3] |= 0x40;
    }
  } else if(mode > RTC_ALARM0_MOD_CNT && last_read == RTC_TYPE_ALARM1) {
    rtc_data[0] &= ~0x80;
    rtc_data[1] &= ~0x80;
    rtc_data[2] &= ~0x80;
    rtc_data[2] &= ~0x40;
    switch (mode) {
      case RTC_ALARM1_MOD_PER_MIN:
        rtc_data[0] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_MIN:
        rtc_data[1] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_HOUR_MIN:
        rtc_data[2] |=  0x80;
      case RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN:
        break;
      case RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN:
        rtc_data[2] |= 0x40;
    }
  }
}
#ifdef __CLOCK_DEBUG__
const char * rtc_alarm_get_mod_str(void)
{
  return rtc_alarm_mode_str[rtc_alarm_get_mode()];
}
#endif
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
void rtc_enable_alarm_int(enum rtc_alarm_index index, bit enable)
{
  if(index == RTC_ALARM0) {
    if(!enable)
      rtc_data[0] &= ~1;
    else
      rtc_data[0] |= 1;
  } else if(index == RTC_ALARM1) {
    if(!enable)
      rtc_data[0] &= ~2;
    else
      rtc_data[0] |= 2;
  }
}

bit rtc_test_alarm_int(enum rtc_alarm_index index)
{
  if(index == RTC_ALARM0) {
    return (rtc_data[0] & 1) == 1;
  } else if(index == RTC_ALARM1) {
    return (rtc_data[0] & 2) == 2;
  }
  return 0;
}

void rtc_clr_alarm_int_flag(enum rtc_alarm_index index)
{
  if(index == RTC_ALARM0) {
    rtc_data[1] &= ~1;
  } else if(index == RTC_ALARM1) {
    rtc_data[1] &= ~2;    
  } 
}

bit rtc_test_alarm_int_flag(enum rtc_alarm_index index)
{
  if(index == RTC_ALARM0) {
    return (rtc_data[1] & 1) == 1;
  } else if(index == RTC_ALARM1) {
    return (rtc_data[1] & 2) == 2;
  }
  return 0;
}

bit rtc_test_eosc(void)
{
  return (rtc_data[0] & 0x80) != 0;
}

void rtc_set_eosc(bit val)
{
  rtc_data[0] &= ~0x80;
  if(val)
    rtc_data[0] |= 0x80;
}

bit rtc_test_bbsqw(void)
{
  return (rtc_data[0] & 0x40) != 0;
}

void rtc_set_bbsqw(bit val)
{
  rtc_data[0] &= ~0x40;
  if(val)
    rtc_data[0] |= 0x40;
}

bit rtc_test_conv(void)
{
  return (rtc_data[0] & 0x20) != 0;
}

void rtc_set_conv(bit val)
{
  rtc_data[0] &= ~0x20;
  if(val)
    rtc_data[0] |= 0x20;
}

enum rtc_square_rate rtc_get_square_rate(void)
{
  return (((rtc_data[0] & 0x18) >> 3) & 0x3);
}

void rtc_set_square_rate(enum rtc_square_rate rt)
{
  unsigned char val = rt;
  rtc_data[0] &= ~0x18;
  rtc_data[0] |= val << 3;
}

#ifdef __CLOCK_DEBUG__
const char * rtc_get_square_rate_str(void)
{
  return rtc_square_rate_str[rtc_get_square_rate()];
}
#endif

bit rtc_test_intcn(void)
{
  return (rtc_data[0] & 0x4) != 0;
}

void rtc_set_intcn(bit val)
{
  rtc_data[0] &= ~0x4;
  if(val)
    rtc_data[0] |= 0x4;
}

bit rtc_test_osf(void)
{
  return (rtc_data[1] & 0x80) != 0;
}

void rtc_set_osf(bit val)
{
  rtc_data[1] &= ~0x80;
  if(val)
    rtc_data[1] |= 0x80;
}

bit rtc_test_en32khz(void)
{
  return (rtc_data[1] & 0x8) != 0;
}

void rtc_set_en32khz(bit val)
{
  rtc_data[1] &= ~0x8;
  if(val)
    rtc_data[1] |= 0x8;
}

bit rtc_test_bsy(void)
{
  return (rtc_data[1] & 0x4) != 0;
}

void rtc_enter_powersave(void)
{ 
  // 停止32KHZ输出
  rtc_read_data(RTC_TYPE_CTL);
  rtc_set_en32khz(0);
  rtc_write_data(RTC_TYPE_CTL);
  
}

void rtc_leave_powersave(void)
{
  // 启动32KHZ输出
  rtc_read_data(RTC_TYPE_CTL);
  rtc_set_en32khz(1);
  rtc_write_data(RTC_TYPE_CTL);
  
}
