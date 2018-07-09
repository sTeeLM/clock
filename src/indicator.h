#ifndef __CLOCK_INDICATOR_H__
#define __CLOCK_INDICATOR_H__

#include "task.h"
/*
1. USB连接，充电中，有电池：红灯长亮
2. USB连接，无电池：绿/黄（绿常亮+红闪烁）交替闪烁
3. USB连接，充电满：绿灯长亮
4. USB连接，电池坏：全灭
5. 充电宝模式启动：绿灯闪烁
6. 引信模式预启动：黄灯闪烁（绿闪烁+红闪烁）
7. 引信模式启动：红灯闪烁
8. 内部中断：蓝灯闪烁
*/

void indicator_initialize (void);

enum indicator_color
{
  INDICATOR_COLOR_RED,
  INDICATOR_COLOR_GREEN,
};

enum indicator_mode
{
  INDICATOR_MODE_BLINK,
  INDICATOR_MODE_ON,
  INDICATOR_MODE_OFF
};

void indicator_clr(void);
void indicator_set(enum indicator_color color, enum indicator_mode mode);

#endif
