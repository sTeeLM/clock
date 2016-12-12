#ifndef __CLOCK_LED_H__
#define __CLOCK_LED_H__

#include "task.h"

void led_initialize (void);

void led_clear(void);
void led_set_blink(unsigned char i);
void led_clr_blink(unsigned char i);
void led_set_dp(unsigned char i);
void led_clr_dp(unsigned char i);
void led_set_code(unsigned char i, char c);
void refresh_led(void);

#endif