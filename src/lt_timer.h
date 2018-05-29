#ifndef __CLOCK_LT_TIMER_H__
#define __CLOCK_LT_TIMER_H__

enum lt_timer_sync_type
{
  LT_TIMER_SYNC_YEAR,
  LT_TIMER_SYNC_MONTH,  
  LT_TIMER_SYNC_DATE,
  LT_TIMER_SYNC_HOUR,
  LT_TIMER_SYNC_MIN,
  LT_TIMER_SYNC_SEC
};

struct lt_timer_struct
{
  /* year mon 仅对绝对时间有意义*/
  unsigned char year;
  unsigned char month;
  /* 如下字段既是相对时间，又被用于绝对时间 */
  unsigned char date; // 0-99
  unsigned char hour; // 0-23
  unsigned char min;  // 0-59
  unsigned char sec;  // 0-59
};

void lt_timer_initialize (void);
void lt_timer_switch_on(void);
void lt_timer_switch_off(void);

void scan_lt_timer(void);

void lt_timer_inc_year(void);
void lt_timer_inc_month(void);
void lt_timer_inc_date(void);
void lt_timer_inc_hour(void);
void lt_timer_inc_min(void);
void lt_timer_inc_sec(void);

unsigned char lt_timer_get_year(void);
unsigned char lt_timer_get_month(void);
unsigned char lt_timer_get_date(void);
unsigned char lt_timer_get_hour(void);
unsigned char lt_timer_get_min(void);
unsigned char lt_timer_get_sec(void);


void lt_timer_save_to_rom(enum lt_timer_sync_type type);
void lt_timer_load_from_rom(void);
void lt_timer_sync_to_rtc(void);
bit lt_timer_get_relative(bit too_close_check);

void lt_timer_dec_sec(void);

void lt_timer_start_rtc(void);
void lt_timer_stop_rtc(void);
void lt_timer_start_ram(void);
void lt_timer_stop_ram(void);

void lt_timer_reset(void);

void lt_timer_switch_display(void);
void lt_timer_display(bit enable);

void lt_timer_enter_powersave(void);
void lt_timer_leave_powersave(void);

#endif