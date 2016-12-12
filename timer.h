#ifndef __CLOCK_TIMER_H__
#define __CLOCK_TIMER_H__

void timer_initialize (float fclk);
extern unsigned char idata counter_1ms;
extern unsigned char idata counter_25ms;
extern unsigned char idata counter_250ms;
extern unsigned char idata counter_1s;

#endif