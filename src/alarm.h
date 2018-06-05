#ifndef __CLOCK_ALARM_H__
#define __CLOCK_ALARM_H__

#include "task.h"

void alarm_initialize (void);
void alarm_enter_powersave(void);
void alarm_leave_powersave(void);
void alarm_proc(enum task_events ev);
void alarm_dump(void);


// 闹钟0是一个可以设置日重复的每日闹铃
struct alarm0_struct {
  unsigned char day_mask;                // 1->enable 0->disable
  unsigned char hour;                    // 0-23
  unsigned char min;                     // 0-59
};

bit alarm0_test_enable(unsigned char day);
void alarm0_set_enable(unsigned char day, bit enable);
unsigned char alarm0_get_day_mask(void);
bit alarm0_get_hour_12(void);
void alarm0_set_hour_12(bit enable);
unsigned char alarm0_get_min(void);
void alarm0_inc_min(void);
unsigned char alarm0_get_hour(void);
void alarm0_inc_hour(void);
//void alarm0_sync_from_rtc(void);
void alarm0_sync_to_rtc(void);

// 闹钟1是整点报时闹钟
bit alarm1_test_enable(void);
void alarm1_set_enable(bit enable);
//void alarm1_sync_from_rtc(void);
void alarm1_sync_to_rtc(void);
void scan_alarm(void);
void alarm_switch_on(void);
void alarm_switch_off(void);
#endif
