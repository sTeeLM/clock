#ifndef __CLOCK_TASK_H__
#define __CLOCK_TASK_H__

// max 32
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
  EV_SCAN_INT_HUB     = 13, // 扫描fuse，hg，mpu
  EV_FUSE0_BROKE      = 14, // fuse0被剪断
  EV_FUSE1_BROKE      = 15, // fuse1被剪断
  EV_FUSE_TRIPWIRE         = 16, // tripwire被剪断
  EV_ROTATE_HG        = 17, // hg检测出倾斜状态改变
  EV_ROTATE_MPU      = 18, // mpu检测出倾斜状态改变	
  EV_ACC_MPU         = 19, // mpu 检测出晃动
  EV_DROP_MPU         = 20, // mpu 检测出下落（失重）
  EV_THERMO_HI     = 21, // 温度太高
  EV_THERMO_LO     = 22, // 温度太低
  EV_FUSE_SEL0         = 23, // fuse 虚拟事件0
  EV_FUSE_SEL1         = 24, // fuse 虚拟事件1
  EV_FUSE_SEL2         = 25, // fuse 虚拟事件2  
  EV_ALARM0           = 26, // 闹钟0应该响起
  EV_ALARM1           = 27, // 闹钟1应该响起
  EV_COUNTER          = 28, // 计时器到时间
  EV_POWER_SAVE       = 29, // 应该进入PS状态 
  EV_COUNT  
};

extern unsigned int idata ev_bits0;
extern unsigned int idata ev_bits1;

extern const char * code task_name[];

typedef void (code *TASK_PROC)(enum task_events);

void task_initialize (void);

void task_dump(void);

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
