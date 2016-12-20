#include "mod_common.h"

// 公共变量，以便于减少DATA使用

unsigned char year_hour;
unsigned char month_min;
unsigned char day_sec;

bit lpress_lock_year_hour;
bit lpress_lock_month_min;
bit lpress_lock_day_sec;

unsigned char lpress_start;

// global flags
unsigned char powersave_timeout; // 0,15,30
bit baoshi;
unsigned char alarm_music_index; // 0, 1, 2
bit is_24;