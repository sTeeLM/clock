#include <string.h>

#include "task.h"

/* hardware*/
#include "timer.h"
#include "rtc.h"
#include "key.h"
#include "led.h"
#include "alarm.h"
#include "power.h"
#include "counter.h"
#include "debug.h"

/* state machine */
#include "sm_display.h"
#include "sm_mod_time.h"
#include "sm_mod_alarm.h"
#include "sm_mod_global_flag.h"
#include "sm_pac_hit.h"
#include "sm_counter.h"
#include "sm_timer.h"
/*

enum task_events
{
  EV_SCAN_KEY         = 0, // 扫描按键
  EV_KEY_MOD_DOWN     = 1, // mod键按下
  EV_KEY_MOD_UP       = 2, // mod键抬起
  EV_KEY_MOD_PRESS    = 3, // mod键短按
  EV_KEY_MOD_LPRESS   = 4, // mod键长按  
  EV_KEY_SET_DOWN     = 5, // set键按下
  EV_KEY_SET_UP       = 6, // set键抬起
  EV_KEY_SET_PRESS    = 7, // set键短按
  EV_KEY_SET_LPRESS   = 8, // set键长按
  EV_KEY_MOD_SET_PRESS    = 9, // mod set键同时短按
  EV_KEY_MOD_SET_LPRESS   = 10, // mod set 键同时长按    
  EV_ALARM            = 11, // 闹钟应该响起
  EV_COUNTER          = 12, // 计时器到时间
  EV_POWER_SAVE       = 13, // 应该进入PS状态 
  EV_250MS            = 14, // 大约每250ms转一下
  EV_1S               = 15, // 大约每1s转一下
  EV_COUNT  
};
*/

static const TASK_PROC code task_procs[EV_COUNT] = 
{
  scan_key_proc,
  mod_proc,
  mod_proc,
  mod_proc,
  mod_proc,  
  set_proc,
  set_proc,
  set_proc,
  set_proc,
  mod_set_proc,
  mod_set_proc,  
  alarm_proc,
  counter_proc,
  power_proc,
  state_machine_timer_proc,
  state_machine_timer_proc  
};

// state machine translate defines
// 超级复杂变态
static const struct sm_trans code sm[] = 
{
  /* SM_DISPLAY */
  // 从别的状态切过来，防止误操作
  {SM_DISPLAY<<4|SM_DISPLAY_INIT, EV_KEY_MOD_UP, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},  
  // 按mod0显示年月日
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, sm_display},
  // 每250ms读取下rtc
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_250MS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // 按mod1进入修改时间模式
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_MOD_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_INIT, sm_mod_time},
  // 闹钟响了
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_ALARM, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, sm_pac_hit},
  // 倒计时到了
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // 该进入节电模式了
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // 进入跑表功能
  {SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, EV_KEY_SET_LPRESS, SM_TIMER<<4|SM_TIMER_INIT, sm_timer},

  // 按mod0显示星期几
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_WEEK, sm_display},
  // 每1s读一下rtc
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, sm_display},
  // 闹钟响了
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_ALARM, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, sm_pac_hit},
  // 倒计时到了
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // 该进入节电模式了
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0回到时分秒显示模式
  {SM_DISPLAY<<4|SM_DISPLAY_YYMMDD, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  
  // 按mod0显示温度
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_TEMP, sm_display},
  // 每1s读一下rtc
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_WEEK, sm_display},
  // 闹钟响了
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_ALARM, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, sm_pac_hit},
  // 倒计时到了
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // 该进入节电模式了
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0回到时分秒显示模式
  {SM_DISPLAY<<4|SM_DISPLAY_WEEK, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  
  // 按mod0回到时分秒显示模式
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // 每1s读一下rtc
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_1S, SM_DISPLAY<<4|SM_DISPLAY_TEMP, sm_display},
  // 闹钟响了
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_ALARM, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, sm_pac_hit},
  // 倒计时到了
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
  // 该进入节电模式了
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_POWER_SAVE, SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, sm_pac_hit},
  // set0回到时分秒显示模式
  {SM_DISPLAY<<4|SM_DISPLAY_TEMP, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},

  /* SM_MODIFY_TIME */
  // 从别的状态进入，防止误操作
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_INIT, EV_KEY_MOD_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time}, 
  // 按mod0进入修改分钟模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // 按set0小时++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // set1小时连续++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // set抬起停止++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // 每250ms读一次rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // mod1 进入修改闹钟模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},
  // mod0 进入修改秒模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // set0 分钟++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // set1 分钟持续++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // set抬起停止++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time},
  // 每250ms读一下rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, sm_mod_time}, 
  // mod1 进入修改闹钟模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MM, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},
  // mod0进入修改年模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // set0将秒清0并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // 每250ms读一次rtc 
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_250MS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, sm_mod_time},
  // mod1进入修改闹钟模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_SS, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},  
  // 按mod0进入修改月模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // set0 年++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // 长按set年持续++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // 抬起set停止年++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // 每秒读取一次rtc  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_1S, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, sm_mod_time},
  // mod1 进入修改闹钟模式  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_YY, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm}, 
  // mod0 进入修改月模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // set0 月++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // 长按set 月持续++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time}, 
  // 抬起set停止月++并写入rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time}, 
  // 每1s读取一下rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_1S, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, sm_mod_time},
  // mod1 进入修改闹钟模式  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_MO, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},   
  // mod0 进入修改小时模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_MOD_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_HH, sm_mod_time},
  // set0 日++
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_PRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // set1 日持续++  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_LPRESS, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // set抬起停止日++写入rtc  
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_SET_UP, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time},
  // 每1s读取一下rtc
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_1S, SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, sm_mod_time}, 
  // mod1 进入修改闹钟模式
  {SM_MODIFY_TIME<<4|SM_MODIFY_TIME_DD, EV_KEY_MOD_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, sm_mod_alarm},

  /* SM_MODIFY_ALARM */
  // 从别的状态进入，防止误操作
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_INIT, EV_KEY_MOD_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // set0小时++  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // set1小时持续++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // set抬起停止++，写入rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_SET_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_alarm},
  // mod0进入修改分钟模式
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_MOD_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm},
  // mod1进入修改全局状态模式
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag},
  // 每1s读取一下rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, EV_1S, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_HH, sm_mod_time}, 
  // set0分钟++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm},
  // set1分钟持续++
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_LPRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm}, 
  // set抬起停止++，写入rtc  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_SET_UP, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, sm_mod_alarm}, 
  // mod0进入打开关闭状态
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_MOD_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, sm_mod_alarm}, 
  // mod1进入修改全局状态模式  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_MM, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag},
  // set0 调整打开关闭，并写入rtc
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, EV_KEY_SET_PRESS, SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, sm_mod_alarm},  
  // mod1进入修改全局状态模式  
  {SM_MODIFY_ALARM<<4|SM_MODIFY_ALARM_ON, EV_KEY_MOD_LPRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, sm_mod_global_flag}, 
  
  /* SM_MODIFY_GLOBAL_FLAG */
  // 防止误操作
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_INIT, EV_KEY_MOD_UP, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag},
  // set0 省电模式超时时间设置
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag},
  // mod0 进入设置整点报时on/off状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, sm_mod_global_flag},  
  // mod1进入显示时间状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display}, 
  // set0 整点报时on/off
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, sm_mod_global_flag},
  // mod0 进入1224小时设置状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, sm_mod_global_flag}, 
  // mod1进入显示时间状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_BS, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display}, 
  // set0 1224模式切换
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_SET_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, sm_mod_global_flag},
  // mod0 进入设置省电模式状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_MOD_PRESS, SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_PS, sm_mod_global_flag}, 
  // mod1 进入显示时间状态
  {SM_MODIFY_GLOBAL_FLAG<<4|SM_MODIFY_GLOBAL_FLAG_1224, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},

  /* SM_PAC_HIT */
  // mod0回到时间显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0回到时间显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display}, 
  // mod0回到时间显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0回到时间显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // mod0回到时间显示模式 
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_KEY_MOD_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // set0回到时间显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_KEY_SET_PRESS, SM_DISPLAY<<4|SM_DISPLAY_HHMMSS, sm_display},
  // 进入闹钟显示模式
  {SM_PAC_HIT<<4|SM_PAC_HIT_POWERSAVE, EV_ALARM, SM_PAC_HIT<<4|SM_PAC_HIT_ALARM, sm_pac_hit},  
  
  /* SM_TIMER */
  // 防止误操作
  {SM_TIMER<<4|SM_TIMER_INIT, EV_KEY_SET_UP, SM_TIMER<<4|SM_TIMER_CLEAR, sm_timer},
  // mod1进入计时器模式
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_MOD_LPRESS, SM_COUNTER<<4|SM_COUNTER_INIT, sm_counter},
  // mod0跑表开始跑
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_RUNNING, sm_timer}, 
  // set0计次
  {SM_TIMER<<4|SM_TIMER_CLEAR, EV_KEY_SET_PRESS, SM_TIMER<<4|SM_TIMER_RUNNING, sm_timer},
  // mod0跑表停止  
  {SM_TIMER<<4|SM_TIMER_RUNNING, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_STOP, sm_timer},
  // mod0跑表清零
  {SM_TIMER<<4|SM_TIMER_STOP, EV_KEY_MOD_PRESS, SM_TIMER<<4|SM_TIMER_CLEAR, sm_timer},
  // set0逐次显示计次
  {SM_TIMER<<4|SM_TIMER_STOP, EV_KEY_SET_PRESS, SM_TIMER<<4|SM_TIMER_STOP, sm_timer}, 
  
  /* SM_COUNTER */
  // 防止误操作
  {SM_COUNTER<<4|SM_COUNTER_INIT, EV_KEY_MOD_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},
  // mod0进入修改分钟状态
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set0小时++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},
  // set1小时快速++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter}, 
  // set抬起小时快速++停止
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, sm_counter},   
  // mod1切回时间显示
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_HH, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},
  // mod0进入修改秒状态
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // set0 分钟++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set1 分钟快速++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // set 抬起分钟快速++停止  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // mod1 回到时间显示  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},  
  // mod0 开始倒计时
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_RUNNING, sm_counter},
  // set0 秒++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // set1 秒快速++
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_LPRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // set抬起秒快速++停止
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_SET_UP, SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, sm_counter},
  // mod1回到时间显示  
  {SM_COUNTER<<4|SM_COUNTER_MODIFY_SS, EV_KEY_MOD_LPRESS, SM_DISPLAY<<4|SM_DISPLAY_INIT, sm_display},
  // set0 暂停倒计时
  {SM_COUNTER<<4|SM_COUNTER_RUNNING, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_STOP, sm_counter}, 
  // set0 继续倒计时
  {SM_COUNTER<<4|SM_COUNTER_STOP, EV_KEY_SET_PRESS, SM_COUNTER<<4|SM_COUNTER_RUNNING, sm_counter},
  // mod0 清除
  {SM_COUNTER<<4|SM_COUNTER_STOP, EV_KEY_MOD_PRESS, SM_COUNTER<<4|SM_COUNTER_MODIFY_MM, sm_counter},
  // 倒计时结束
  {SM_COUNTER<<4|SM_COUNTER_RUNNING, EV_COUNTER, SM_PAC_HIT<<4|SM_PAC_HIT_COUNTER, sm_pac_hit},
};



unsigned int idata ev_bits;
static unsigned char sm_state; // hi 4 bits : state, lo 4 bits: sub-state 

void task_initialize (void)
{
  CDBG("task_initialize\n");
  ev_bits = 0;
  sm_state = SM_DISPLAY|SM_DISPLAY_INIT;
  set_task(EV_KEY_MOD_UP);
}

void run_task(void)
{
  unsigned char c;
  while(1) {
    for(c = 0; c < EV_COUNT; c++) {
      if(test_task(c)) {
        task_procs[c](c);
        clr_task(c);
      }
    }
  }
}

void state_machine_timer_proc(enum task_events ev)
{
  run_state_machine(ev);
}

void run_state_machine(enum task_events ev)
{
  unsigned char c;
  
  CDBG("run_state_machine %bd %bd|%bd\n", ev, get_sm_state(sm_state), get_sm_ss_state(sm_state));
  for (c = 0 ; c < sizeof(sm)/sizeof(struct sm_trans) ; c++) {
    if(sm_state == sm[c].from_state && ev == sm[c].event) {
      CDBG("SM: %bd %bd %bd|%bd -> %bd|%bd\n", c, ev,
        get_sm_state(sm_state), get_sm_ss_state(sm_state), 
        get_sm_state(sm[c].to_state), get_sm_ss_state(sm[c].to_state));
      sm[c].sm_proc(sm_state, sm[c].to_state, ev);
      sm_state = sm[c].to_state;
      break;
    }
  }
  
}
