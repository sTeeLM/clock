#ifndef __CLOCK_LT_TIMER_H__
#define __CLOCK_LT_TIMER_H__

enum lt_timer_sync_type
{
  LT_TIMER_SYNC_DAY,
  LT_TIMER_SYNC_HOUR,
  LT_TIMER_SYNC_MIN,
  LT_TIMER_SYNC_SEC
};

struct lt_timer_struct
{
  unsigned char day;  // 0-99
  unsigned char hour; // 0-23
  unsigned char min;  // 0-59
  unsigned char sec;  // 0-59
};

void lt_timer_initialize (void);
void lt_timer_switch_on(void);
void lt_timer_switch_off(void);

void scan_lt_timer(void);

void lt_timer_inc_day(void);
void lt_timer_inc_hour(void);
void lt_timer_inc_min(void);
void lt_timer_inc_sec(void);

unsigned char lt_timer_get_hour(void);
unsigned char lt_timer_get_min(void);
unsigned char lt_timer_get_sec(void);
unsigned char lt_timer_get_day(void);

void lt_timer_sync_to_rom(enum lt_timer_sync_type type);
void lt_timer_sync_from_rom(void);
bit lt_timer_is_overflow(void);
#endif