#ifndef __CLOCK_SM_DISPLAY_H__
#define __CLOCK_SM_DISPLAY_H__

#include "sm.h"

enum sm_display_ss {
  SM_DISPLAY_INIT   = 0, // 初始状态
  SM_DISPLAY_HHMMSS = 1, // 显示时分秒
  SM_DISPLAY_YYMMDD = 2, // 显示年月日
  SM_DISPLAY_WEEK   = 3, // 显示星期几
  SM_DISPLAY_TEMP   = 4, // 显示温度
};

void sm_display(unsigned char from, unsigned char to, enum task_events);

#endif