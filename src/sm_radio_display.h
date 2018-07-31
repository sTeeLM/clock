#ifndef __CLOCK_SM_RADIO_DISPLAY_H__
#define __CLOCK_SM_RADIO_DISPLAY_H__

#include "sm.h"

enum sm_radio_display_ss {
  SM_RADIO_DISPLAY_INIT       = 0, // 初始状态
  SM_RADIO_DISPLAY_STATION    = 1, // 电台显示和调节
  SM_RADIO_DISPLAY_VOLUME     = 2, // 音量显示和调节
  SM_RADIO_DISPLAY_MOD        = 3, // 其他参数调节
};

extern const char * code sm_radio_display_ss_name[];

void sm_radio_display_init(unsigned char from, unsigned char to, enum task_events);
void sm_radio_display_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_radio_display_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_radio_display_submod2(unsigned char from, unsigned char to, enum task_events);

#endif
