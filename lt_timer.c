#include "lt_timer.h"
#include "debug.h"
#include "clock.h"
#include "rom.h"

#define LT_TIMER_MAX_DAY 100

static struct lt_timer_struct ltm; 

void lt_timer_initialize (void)
{
  CDBG("lt_timer_initialize\n");
}

void lt_timer_switch_on(void)
{
  CDBG("lt_timer_switch_on\n");
}

void lt_timer_switch_off(void)
{
  CDBG("lt_timer_switch_off\n");
}

void scan_lt_timer(void)
{
  CDBG("scan_lt_timer\n");
}

void lt_timer_inc_day(void)
{
  ltm.day = (++ ltm.day) % LT_TIMER_MAX_DAY;
}

void lt_timer_inc_hour(void)
{
  ltm.hour = (++ ltm.hour) % 24;
}

void lt_timer_inc_min(void)
{
  ltm.min = (++ ltm.min) % 60;
}

void lt_timer_inc_sec(void)
{
  ltm.sec = (++ ltm.sec) % 60;
}

unsigned char lt_timer_get_hour(void)
{
  return ltm.hour;
}

unsigned char lt_timer_get_min(void)
{
  return ltm.min;
}

unsigned char lt_timer_get_sec(void)
{
  return ltm.sec;
}

unsigned char lt_timer_get_day(void)
{
  return ltm.day;
}

// test current time + ltm > 2099-12-31-23:59:59
bit lt_timer_is_overflow(void)
{
  bit ret = 0;
  
  unsigned char sec, min, hour, date, mon, year;
  
  clock_enable_interrupt(0);
  
  clock_enable_interrupt(1);
  
  return ret;
}

void lt_timer_sync_to_rom(enum lt_timer_sync_type type)
{
  switch (type) { 
    case LT_TIMER_SYNC_DAY:
      rom_write(ROM_LT_TIMER_DAY, ltm.day);
      break;
    case LT_TIMER_SYNC_HOUR:
      rom_write(ROM_LT_TIMER_HOUR, ltm.hour);
      break;
    case LT_TIMER_SYNC_MIN:
      rom_write(ROM_LT_TIMER_MIN, ltm.min);
      break;
    case LT_TIMER_SYNC_SEC:
      rom_write(ROM_LT_TIMER_SEC, ltm.sec);
      break;
    
  }
}

void lt_timer_sync_from_rom(void)
{
  ltm.day = rom_read(ROM_LT_TIMER_DAY);
  ltm.hour = rom_read(ROM_LT_TIMER_HOUR);
  ltm.min  = rom_read(ROM_LT_TIMER_MIN);
  ltm.sec  = rom_read(ROM_LT_TIMER_SEC);
}
