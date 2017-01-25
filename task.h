#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

enum task_events
{
  EV_250MS            = 0, // 大约每250ms转一下
  EV_1S               = 1, // 大约每1s转一下  
  EV_SCAN_KEY         = 2, // 扫描按键
  EV_KEY_MOD_DOWN     = 3, // mod键按下
  EV_KEY_MOD_UP       = 4, // mod键抬起
  EV_KEY_MOD_PRESS    = 5, // mod键短按
  EV_KEY_MOD_LPRESS   = 6, // mod键长按  
  EV_KEY_SET_DOWN     = 7, // set键按下
  EV_KEY_SET_UP       = 8, // set键抬起
  EV_KEY_SET_PRESS    = 9, // set键短按
  EV_KEY_SET_LPRESS   = 10, // set键长按
  EV_KEY_MOD_SET_PRESS    = 11, // mod set键同时短按
  EV_KEY_MOD_SET_LPRESS   = 12, // mod set 键同时长按 
  EV_SCAN_INT_HUB     = 13, // 扫描fuse，hg，gyro
  EV_FUSE0_SHORT_BROKE = 14, // fuse被剪断或者短路
  EV_FUSE1_SHORT_BROKE = 15, // fuse被剪断或者短路
  EV_HG            = 16, // hg检测出来倾斜状态改变
  EV_GYRO          = 17, // gyro检测出来倾斜状态改变
  EV_THERMO_LO     = 18, // 温度太低
  EV_THERMO_HI     = 19, // 温度太高
  EV_FUSE0_TRIGGER     = 20, // fuse应该被触发 
  EV_FUSE1_TRIGGER     = 21,
  EV_TRIPWIRE         = 22,
  EV_ALARM0           = 23, // 闹钟0应该响起
  EV_ALARM1           = 24, // 闹钟1应该响起
  EV_COUNTER          = 25, // 计时器到时间
  EV_POWER_SAVE       = 26, // 应该进入PS状态 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);


// 这些宏也在中断里被调用，所以不能是带参数函数，职能拿宏实现了
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