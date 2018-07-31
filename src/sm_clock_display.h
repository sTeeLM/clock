#ifndef __CLOCK_SM_CLOCK_DISPLAY_H__
#define __CLOCK_SM_CLOCK_DISPLAY_H__

#include "sm.h"

enum sm_clock_display_ss {
  SM_CLOCK_DISPLAY_INIT   = 0, // 初始状态
  SM_CLOCK_DISPLAY_HHMMSS = 1, // 显示时分秒
  SM_CLOCK_DISPLAY_YYMMDD = 2, // 显示年月日
  SM_CLOCK_DISPLAY_WEEK   = 3, // 显示星期几
  SM_CLOCK_DISPLAY_TEMP   = 4, // 显示温度
};

extern const char * code sm_clock_display_ss_name[];

void sm_clock_display_init(unsigned char from, unsigned char to, enum task_events);
void sm_clock_display_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_clock_display_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_clock_display_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_clock_display_submod3(unsigned char from, unsigned char to, enum task_events);
#endif
