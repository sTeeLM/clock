#ifndef __CLOCK_SM_H__
#define __CLOCK_SM_H__

#include "task.h"

// max 16
// SM_CLOCK
enum sm_states_clock
{
  SM_CLOCK_DISPLAY      = 0, // 一般显示状态
  SM_CLOCK_MODIFY_TIME  = 1, // 修改时间
  SM_CLOCK_MODIFY_ALARM = 2, // 修改闹钟
  SM_CLOCK_POWERSAVE    = 3, // 节电
  SM_CLOCK_ALARM        = 4, // 闹钟/整点报时
  SM_CLOCK_TIMER        = 5, // 秒表功能
  SM_CLOCK_COUNTER      = 6, // 倒计时功能
};

// max 16
// SM_POWER_PACK
enum sm_states_power_pack
{
  SM_POWER_PACK_DISPLAY = 0,  // 电量展示
  SM_POWER_PACK_POWERSAVE = 1,// 节电模式
};

// max 16
enum sm_states_radio
{
  SM_RADIO_DISPLAY = 0,  // 播音，调节电台、音量
  SM_RADIO_MOD     = 1,  // 其他参数设置
  SM_RADIO_POWERSAVE = 2,// 节电模式
};

// max 16
// SM_FUSE
enum sm_stats_fuse
{
  SM_FUSE_TEST          = 0, // fuse测试
  SM_FUSE_PARAM         = 1,//  fuse参数设置
  SM_FUSE_TIMER         = 2,//  fuse定时模式
  SM_FUSE_DETONATE      = 3,// boom!
  SM_FUSE_POWERSAVE     = 4,// 节电
};

// max 16
// SM_GLOBAL_FLA
enum sm_states_global_flag
{
  SM_GLOBAL_FLAG_MODIFY = 0, // 修改全局参数
};

// max 256
enum sm_tables
{
  SM_CLOCK  = 0,
  SM_POWER_PACK,
  SM_RADIO,
  SM_FUSE,
  SM_GLOBAL_FLAG,
  SM_TABLE_CNT
};

typedef void (code *SM_PROC)(unsigned char from, unsigned char to, enum task_events ev);

struct sm_trans
{
  unsigned char from_table;  
  unsigned char from_state;
  enum task_events event;
  unsigned char to_table;
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

void sm_show_current(void);
void sm_dump(void);
void sm_dump_table(void);
void sm_dump_state(void);
void sm_dump_sub_state(void);
bit sm_set_table_by_name(const char * table_name);
bit sm_set_state_by_name(const char * state_name);
bit sm_set_sub_state_by_name(const char * sub_state_name);

extern unsigned char sm_curr_table; // current table;
extern unsigned char sm_new_table;

#endif
