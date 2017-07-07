#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_250MS            = 0, // 大约每250ms转一下
  EV_1S               = 1, // 大约每1s转一下  
  EV_SCAN_KEY         = 2, // 扫描按键 
  EV_KEY_SET_DOWN     = 3, // set键按下
  EV_KEY_SET_PRESS    = 4, // set键短按
  EV_KEY_SET_LPRESS   = 5, // set键长按
  EV_KEY_SET_UP       = 6, // set键抬起
  EV_KEY_MOD_DOWN     = 7, // mod键按下
  EV_KEY_MOD_PRESS    = 8, // mod键短按
  EV_KEY_MOD_LPRESS   = 9, // mod键长按
  EV_KEY_MOD_UP       = 10, // mod键抬起 
  EV_KEY_MOD_SET_PRESS    = 11, // mod set 键同时短按
  EV_KEY_MOD_SET_LPRESS   = 12, // mod set 键同时长按 
  EV_SCAN_INT_HUB     = 13, // 扫描fuse，hg，gyro
  EV_FUSE0_SHORT      = 14, // fuse0被短路
  EV_FUSE0_BROKE      = 15, // fuse0被剪断
  EV_FUSE1_SHORT      = 16, // fuse0被短路
  EV_FUSE1_BROKE      = 17, // fuse0被剪断
  EV_ROTATE_HG        = 18, // hg检测出倾斜状态改变
  EV_ROTATE_GYRO      = 19, // gyro检测出倾斜状态改变	
  EV_ACC_GYRO         = 20, // gyro 检测出晃动
  EV_DROP_GYRO         = 21, // gyro 检测出下落（失重）
  EV_THERMO_HI     = 22, // 温度太高
  EV_THERMO_LO     = 23, // 温度太低
  EV_TRIPWIRE         = 24, // tripwire被剪断
  EV_FUSE_SEL0         = 25, // fuse 虚拟事件0
  EV_FUSE_SEL1         = 26, // fuse 虚拟事件1
  EV_ALARM0           = 27, // 闹钟0应该响起
  EV_ALARM1           = 28, // 闹钟1应该响起
  EV_COUNTER          = 29, // 计时器到时间
  EV_POWER_SAVE       = 30, // 应该进入PS状态 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);


// 这些宏也在中断里被调用，所以不能是带参数函数，只能拿宏实现了
#define set_task(ev1)             \
  do{                             \
    if(ev1 < 16)                  \
      ev_bits0 |= 1<<ev1;         \
    else                          \
      ev_bits1 |= 1<<(ev1 - 16);  \
  }while(0)

#define clr_task(ev1)               \
  do{                               \
    if(ev1 < 16)                    \
      ev_bits0 &= ~(1<<ev1);        \
    else                            \
      ev_bits1 &= ~(1<<(ev1 - 16)); \  
  }while(0)
    
#define  test_task(ev1)             \
  (ev1 < 16 ? (ev_bits0 & (1<<ev1)) : (ev_bits1 & (1<<(ev1 - 16))))

void run_task(void);

#endif