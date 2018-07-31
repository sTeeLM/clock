#ifndef __CLOCK_SM_GLOBAL_FLAG_MOD_H__
#define __CLOCK_SM_GLOBAL_FLAG_MOD_H__

#include "sm.h"

enum sm_global_flag_mod_ss {
  SM_GLOBAL_FLAG_MODIFY_INIT   = 0, // 初始状态
  SM_GLOBAL_FLAG_MODIFY_PS     = 1, // 节电模式超时时间切换：0，15，30
  SM_GLOBAL_FLAG_MODIFY_BEEP   = 2, // 按键音
  SM_GLOBAL_FLAG_MODIFY_1224   = 3, // 12/24小时显示切换
  SM_GLOBAL_FLAG_MODIFY_REMOTE = 4  // 遥控器
};

extern const char * code sm_global_flag_mod_ss_name[];

void sm_global_flag_mod_init(unsigned char from, unsigned char to, enum task_events);
void sm_global_flag_mod_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_global_flag_mod_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_global_flag_mod_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_global_flag_mod_submod3(unsigned char from, unsigned char to, enum task_events);

#endif
