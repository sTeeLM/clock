#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

void timer_initialize (float fclk);
void timer_enter_powersave(void);
void timer_leave_powersave(void);

extern unsigned char idata counter_1ms;
extern unsigned char idata counter_25ms;
extern unsigned char idata counter_250ms;
extern unsigned char idata counter_1s;

#endif