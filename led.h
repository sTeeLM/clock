#ifndef __CLOCK_LED_H__
#define __CLOCK_LED_H__

#include "task.h"

void led_initialize (void);

void led_set_blink(unsigned char i);
void led_clr_blink(unsigned char i);
void led_set_dig(unsigned char i);
void led_clr_dig(unsigned char i);
void led_set_code(unsigned char i, char c);

extern char led_data[6];
extern char led_blink;
extern char led_index;
extern unsigned char scan_loop_cnt;
#endif