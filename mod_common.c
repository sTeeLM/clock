#include "mod_common.h"
#include "debug.h"
#include "led.h"
#include "beeper.h"

// 公共变量，以便于减少DATA使用

bit lpress_lock_year_hour;
bit lpress_lock_month_min;
bit lpress_lock_day_sec;

unsigned char lpress_start;
unsigned char last_display_s;


void display_logo(unsigned char fun)
{
  CDBG("display_logo\n");
  led_clear();
  led_set_code(5, 'F');
  led_set_code(4, 'U');
  led_set_code(3, 'N');
  led_set_code(1, (fun / 10) + 0x30);
  led_set_code(0, (fun % 10) + 0x30);
  beeper_beep();
}