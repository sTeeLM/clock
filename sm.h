#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

enum sm_states
{
  SM_CLOCK_DISPLAY      = 0, // 一般显示状态
  SM_CLOCK_MODIFY_TIME        = 1, // 修改时间
  SM_CLOCK_MODIFY_ALARM = 2, // 修改闹钟
  SM_CLOCK_MODIFY_GLOBAL_FLAG = 3, // 修改全局状态
  SM_CLOCK_POWERSAVE    = 4, // 节电
  SM_CLOCK_ALARM        = 5, // 闹钟/整点报时
  SM_CLOCK_TIMER        = 6, // 秒表功能
  SM_CLOCK_COUNTER      = 7, // 倒计时功能
  SM_FUSE_TEST          = 8, // fuse测试
  SM_FUSE_MODE          = 9, // fuse模式选择
  SM_FUSE_PARAM         = 10,// fuse参数设置
  SM_FUSE_TIMER         = 11,// fuse定时模式
  SM_FUSE_GRENADE       = 12,// fuse触碰模式
  SM_FUSE_DETONATE      = 13,// boom!
  SM_FUSE_POWERSAVE     = 14,// 节电
};


typedef void (code *SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_state;
  enum task_events event;
  unsigned char to_state;
  SM_PROC sm_proc;
};


#define get_sm_state(st) \
  (((st & 0xF0) >> 4) & 0x0F)

#define get_sm_ss_state(st) \
  (st & 0x0F)

void run_state_machine(enum task_events);
void null_proc(enum task_events ev);
void sm_initialize (void);

#endif