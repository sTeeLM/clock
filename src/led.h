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
void led_clr_all_blink(void);
void led_set_dp(unsigned char i);
void led_clr_dp(unsigned char i);
void led_set_code(unsigned char i, char c);
void refresh_led(void);

// 由于需要刷新timer数据，在这里开个口，别人不要用
extern unsigned char idata led_data[];
extern unsigned char code led_code[];
extern unsigned char idata led_blink;
extern bit led_powersave;

void led_save(void);
void led_restore(void);
#endif
