#ifndef __CLOCK_RTC_H__
#define __CLOCK_RTC_H__

enum rtc_data_type {
  RTC_TYPE_TIME    = 0, // ʱ��
  RTC_TYPE_DATE    = 1, // ����
  RTC_TYPE_ALARM0  = 2, // ����0
  RTC_TYPE_ALARM1  = 3, // ����1
  RTC_TYPE_TEMP    = 4, // �¶�
  RTC_TYPE_CTL     = 5,
  RTC_TYPE_USR0    = 6, // �û��Զ���0
  RTC_TYPE_USR1    = 7, // �û��Զ���1
};

void rtc_initialize (void);
void rtc_read_data(enum rtc_data_type type);
void rtc_write_data(enum rtc_data_type type);

// ��read_rtc_data(RTC_TYPE_TIME)֮�����
unsigned char rtc_time_get_hour(void);
void rtc_time_set_hour(unsigned char hour);
void rtc_time_set_hour_12(unsigned char enable);
unsigned char rtc_time_get_hour_12();
unsigned char rtc_time_get_min(void);
void rtc_time_set_min(unsigned char min);
unsigned char rtc_time_get_sec(void);
void rtc_time_set_sec(unsigned char sec);

// ��rtc_read_data(RTC_TYPE_DATE)֮�����
unsigned char rtc_date_get_year();
void rtc_date_set_year(unsigned char year);
unsigned char rtc_date_get_month();
void rtc_date_set_month(unsigned char month);
unsigned char rtc_date_get_day();
bit rtc_date_set_day(unsigned char day);
unsigned char rtc_date_get_week();

// ��rtc_read_data(RTC_TYPE_ALARM0)����RTC_TYPE_ALARM1֮�����
unsigned char rtc_alarm_get_hour();
unsigned char rtc_alarm_get_hour_12();
void rtc_alarm_set_hour_12(unsigned char enable);
void rtc_alarm_set_hour(unsigned char hour);
unsigned char rtc_alarm_get_min();
void rtc_alarm_set_min( unsigned char min);

// ��rtc_read_data(RTC_TYPE_TEMP)֮�����
bit rtc_get_temperature(unsigned char * integer, unsigned char * flt);

// ��rtc_read_data��RTC_TYPE_CTL��֮�����
bit rtc_test_alarm_int(unsigned char index);
void rtc_enable_alarm_int(bit enable, unsigned char index);
void rtc_clr_alarm_int(unsigned char index);
#endif