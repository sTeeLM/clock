#ifndef __CLOCK_RTC_H__
#define __CLOCK_RTC_H__

enum rtc_data_type {
  RTC_TYPE_TIME    = 0, // 时间
  RTC_TYPE_DATE    = 1, // 日期
  RTC_TYPE_ALARM0  = 2, // 闹钟0
  RTC_TYPE_ALARM1  = 3, // 闹钟1
  RTC_TYPE_TEMP    = 4, // 温度
  RTC_TYPE_CTL     = 5,
};

enum rtc_alarm_mode
{
  RTC_ALARM0_MOD_PER_SEC                 = 0,
  RTC_ALARM0_MOD_MATCH_SEC               = 1,  
  RTC_ALARM0_MOD_MATCH_MIN_SEC           = 2, 
  RTC_ALARM0_MOD_MATCH_HOUR_MIN_SEC      = 3, 
  RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC = 4,   
  RTC_ALARM0_MOD_MATCH_DAY_HOUR_MIN_SEC  = 5,
  RTC_ALARM0_MOD_CNT                     = 6, 
  RTC_ALARM1_MOD_PER_MIN                 = 7,
  RTC_ALARM1_MOD_MATCH_MIN               = 8,  
  RTC_ALARM1_MOD_MATCH_HOUR_MIN          = 9, 
  RTC_ALARM1_MOD_MATCH_DATE_HOUR_MIN     = 10,   
  RTC_ALARM1_MOD_MATCH_DAY_HOUR_MIN      = 11,     
};

void rtc_initialize (void);

bit rtc_is_lt_timer(void);
void rtc_set_lt_timer(bit enable);
void scan_rtc(void);

void rtc_enter_powersave(void);
void rtc_leave_powersave(void);

void rtc_read_data(enum rtc_data_type type);
void rtc_write_data(enum rtc_data_type type);

// 在read_rtc_data(RTC_TYPE_TIME)之后调用
unsigned char rtc_time_get_hour(void);
void rtc_time_set_hour(unsigned char hour);
void rtc_time_set_hour_12(bit enable);
bit rtc_time_get_hour_12(void);
unsigned char rtc_time_get_min(void);
void rtc_time_set_min(unsigned char min);
unsigned char rtc_time_get_sec(void);
void rtc_time_set_sec(unsigned char sec);

// 在rtc_read_data(RTC_TYPE_DATE)之后调用
unsigned char rtc_date_get_year(void);
void rtc_date_set_year(unsigned char year);
unsigned char rtc_date_get_month(void);
void rtc_date_set_month(unsigned char month);
unsigned char rtc_date_get_date(void);
bit rtc_date_set_date(unsigned char date);
unsigned char rtc_date_get_day(void);
void rtc_date_set_day(unsigned char day);

// 在rtc_read_data(RTC_TYPE_ALARM0)或者RTC_TYPE_ALARM1之后调用
bit rtc_alarm_get_hour_12();
void rtc_alarm_set_hour_12(bit enable);
unsigned char rtc_alarm_get_day(void);
void rtc_alarm_set_day(unsigned char day);
unsigned char rtc_alarm_get_date(void);
void rtc_alarm_set_date(unsigned char date);
unsigned char rtc_alarm_get_hour(void);
void rtc_alarm_set_hour(unsigned char hour);
unsigned char rtc_alarm_get_min();
void rtc_alarm_set_min( unsigned char min);
unsigned char rtc_alarm_get_sec();
void rtc_alarm_set_sec( unsigned char sec);
enum rtc_alarm_mode rtc_alarm_get_mod(void);
void rtc_alarm_set_mode(enum rtc_alarm_mode mode);
enum rtc_alarm_mode rtc_alarm_get_mode(void);

// 在rtc_read_data(RTC_TYPE_TEMP)之后调用
bit rtc_get_temperature(unsigned char * integer, unsigned char * flt);

enum rtc_alarm_index {
  RTC_ALARM0 = 0,
  RTC_ALARM1
};

// 在rtc_read_data（RTC_TYPE_CTL）之后调用
void rtc_enable_alarm_int(enum rtc_alarm_index index, bit enable);
bit rtc_test_alarm_int(enum rtc_alarm_index index);
bit rtc_test_alarm_int_flag(enum rtc_alarm_index index);
void rtc_clr_alarm_int_flag(enum rtc_alarm_index index);

#endif