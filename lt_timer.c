#include "lt_timer.h"
#include "debug.h"
#include "clock.h"
#include "rom.h"

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

void lt_timer_inc_date(void)
{
  ltm.date = (++ ltm.date) % clock_get_mon_date(ltm.year, ltm.mon);
}

void lt_timer_inc_hour(void)
{
  ltm.hour = (++ ltm.hour) % 24;
}

void lt_timer_inc_year(void)
{
  ltm.year = (++ ltm.year) % 100;
}

void lt_timer_inc_min(void)
{
  ltm.min = (++ ltm.min) % 60;
}

void lt_timer_inc_month(void)
{
  ltm.mon = (++ ltm.mon) % 12;
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

unsigned char lt_timer_get_year(void)
{
  return ltm.year;
}

unsigned char lt_timer_get_month(void)
{
  return ltm.mon + 1;
}

unsigned char lt_timer_get_date(void)
{
  return ltm.date + 1;
}

void lt_timer_sync_to_rom(enum lt_timer_sync_type type)
{
  switch (type) {
    case LT_TIMER_SYNC_YEAR:
      rom_write(ROM_LT_TIMER_SYNC_YEAR, ltm.year);
      break;
    case LT_TIMER_SYNC_MON:
      rom_write(ROM_LT_TIMER_SYNC_MON, ltm.mon);
      break; 
    case LT_TIMER_SYNC_DATE:
      rom_write(ROM_LT_TIMER_SYNC_DATE, ltm.date);
      break;
    case LT_TIMER_SYNC_HOUR:
      rom_write(ROM_LT_TIMER_SYNC_HOUR, ltm.hour);
      break;
    case LT_TIMER_SYNC_MIN:
      rom_write(ROM_LT_TIMER_SYNC_MIN, ltm.min);
      break;
    case LT_TIMER_SYNC_SEC:
      rom_write(ROM_LT_TIMER_SYNC_SEC, ltm.sec);
      break;
    
  }
}

void lt_timer_sync_from_rom(void)
{
  ltm.year = rom_read(ROM_LT_TIMER_SYNC_YEAR);
  ltm.mon  = rom_read(ROM_LT_TIMER_SYNC_MON);
  ltm.date = rom_read(ROM_LT_TIMER_SYNC_DATE);
  ltm.hour = rom_read(ROM_LT_TIMER_SYNC_HOUR);
  ltm.min  = rom_read(ROM_LT_TIMER_SYNC_MIN);
  ltm.sec  = rom_read(ROM_LT_TIMER_SYNC_SEC);
}
