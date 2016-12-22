#ifndef __CLOCK_LED_H__
#define __CLOCK_LED_H__

#include "task.h"


#define LED_CODE_BLACK 0xFF
#define LED_CODE_CEL   0 // degree Celsius

void led_initialize (void);
void led_enter_powersave(void);
void led_leave_powersave(void);
void led_clear(void);
void led_set_blink(unsigned char i);
void led_clr_blink(unsigned char i);
void led_set_dp(unsigned char i);
void led_clr_dp(unsigned char i);
void led_set_code(unsigned char i, char c);
void refresh_led(void);

#endif