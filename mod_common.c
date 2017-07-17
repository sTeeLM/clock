#include "mod_common.h"
#include "debug.h"
#include "led.h"
#include "beeper.h"

// �����������Ա��ڼ���DATAʹ��

bit lpress_lock_year_hour;
bit lpress_lock_month_min;
bit lpress_lock_day_sec;

unsigned char lpress_start;
unsigned char last_display_s;
unsigned char common_state;

void display_logo(enum display_logo_type type, unsigned char num)
{
  CDBG("display_logo %bd %bd\n", type, num);
	
  led_clear();
	if(type == DISPLAY_LOGO_TYPE_CLOCK) {
		led_set_code(5, 'C');
	} else {
		led_set_code(5, 'F');
	}
	led_set_code(1, (num / 10) + 0x30);
	led_set_code(0, (num % 10) + 0x30);
  beeper_beep_beep_always();
}