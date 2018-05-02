#include "lt_timer.h"
#include "debug.h"
#include "clock.h"
#include "rom.h"
#include "rtc.h"
#include "led.h"
#include "task.h"
#include "power.h"

static bit lt_tmr_start;
static bit lt_tmr_stopped;
static bit lt_tmr_display;
static bit lt_tmr_disp_day;

#define LT_TIMER_MAX_DAY 100 // 最多100天，反正电池最多也只能坚持3个月？
#define LT_MIN_SEC  30 // 必须至少提前30s


static struct lt_timer_struct idata ltm; 

void lt_timer_initialize (void)
{
  CDBG("lt_timer_initialize\n");
  lt_tmr_start    = 0;
  lt_tmr_stopped  = 1;
  lt_tmr_display  = 0;
  lt_tmr_disp_day = 0;
}

void lt_timer_enter_powersave(void)
{
  CDBG("lt_timer_enter_powersave\n");
  if(rtc_is_lt_timer()) {
    lt_timer_stop_ram();
    lt_timer_start_rtc();
  }
}

void lt_timer_leave_powersave(void)
{
  CDBG("lt_timer_leave_powersave\n");
  if(rtc_is_lt_timer()) {
    lt_timer_stop_rtc();
    lt_timer_sync_from_rom();
    if(!lt_timer_get_relative(0)) {
      lt_tmr_stopped = 1;
      set_task(EV_COUNTER);
    } else {
      lt_timer_start_ram();
    }
  }
}

void lt_timer_switch_on(void)
{
  CDBG("lt_timer_switch_on\n");
  lt_timer_sync_to_rtc();
}

void lt_timer_switch_off(void)
{
  CDBG("lt_timer_switch_off\n");
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, 0);
  rtc_enable_alarm_int(RTC_ALARM1, 0);
  rtc_write_data(RTC_TYPE_CTL);
}

void scan_lt_timer(void)
{
  bit alarm0_hit;
  
  CDBG("scan_lt_timer\n");
  
  rtc_read_data(RTC_TYPE_CTL);
  alarm0_hit = rtc_test_alarm_int_flag(RTC_ALARM0);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
  rtc_write_data(RTC_TYPE_CTL);
  
  if(alarm0_hit) {
    rtc_read_data(RTC_TYPE_CTL);
    
    rtc_read_data(RTC_TYPE_DATE);
    if(rtc_date_get_year() == ltm.year) {
      if(power_test_flag()) {
        power_clr_flag();
      }
      set_task(EV_COUNTER);
    }
  }
}

void lt_timer_inc_year(void)
{
  ltm.year = (++ ltm.year) % 99;
}

void lt_timer_inc_month(void)
{
  ltm.month = (++ ltm.month) % 12;
}

void lt_timer_inc_date(void)
{
  ltm.date = (++ ltm.date) % LT_TIMER_MAX_DAY;
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

unsigned char lt_timer_get_year(void)
{
  return ltm.year;
}

unsigned char lt_timer_get_month(void)
{
  return ltm.month + 1;
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

unsigned char lt_timer_get_date(void)
{
  return ltm.date + 1;
}

void lt_timer_start_rtc(void)
{
  CDBG("lt_timer_start_rtc\n");
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, 1);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
  rtc_write_data(RTC_TYPE_CTL);
}

void lt_timer_stop_rtc(void)
{
  CDBG("lt_timer_stop_rtc\n");
  rtc_read_data(RTC_TYPE_CTL);
  rtc_enable_alarm_int(RTC_ALARM0, 0);
  rtc_clr_alarm_int_flag(RTC_ALARM0);
  rtc_write_data(RTC_TYPE_CTL);
}

void lt_timer_start_ram(void)
{
  CDBG("lt_timer_start_ram\n");
  lt_tmr_start   = 1;
  lt_tmr_stopped = 0;
}

void lt_timer_stop_ram(void)
{
  CDBG("lt_timer_stop_ram\n");
  lt_tmr_start = 0;
}

void lt_timer_switch_display(void)
{
  lt_tmr_disp_day = !lt_tmr_disp_day;
  CDBG("lt_timer_switch_display %bd\n", lt_tmr_disp_day ? 1 : 0);
}

void lt_timer_display(bit enable)
{
  CDBG("lt_timer_display %bd\n", enable ? 1 : 0);
  lt_tmr_display = enable;
}

void lt_timer_reset(void)
{
  lt_timer_stop_rtc();
  lt_timer_stop_ram();
  lt_tmr_start    = 0;
  lt_tmr_stopped  = 1;
  lt_tmr_display  = 0;
  lt_tmr_disp_day = 0;  
}

//////// sub year/month/date/hour/min/sec and clk, 结果放在ltm
// return borrow min
static bit lt_timer_sub_sec_min(void)
{
  // 减sec
  bit borrow_min = 0;
  
  if(ltm.sec < clock_get_sec()) {
    // borrow 1 min
    ltm.sec += 60;
    borrow_min = 1;
  }
  
  ltm.sec -= clock_get_sec();

  return borrow_min;
}


// return borrow hour
static bit lt_timer_sub_min_hour(bit borrow_min)
{
  // 减min
  bit borrow_hour = 0;
  
  if(ltm.min < clock_get_min() + (borrow_min ? 1 : 0)) {
    // borrow 1 hour
    ltm.min += 60;
    borrow_hour = 1;
  }
  
  ltm.min -= clock_get_min();
  ltm.min -= (borrow_min ? 1 : 0);
  
  return borrow_hour;
}

// return borrow date
static bit lt_timer_sub_hour_date(bit borrow_hour)
{
  // 减hour
  bit borrow_date = 0;
  
  if(ltm.hour < clock_get_hour() + (borrow_hour ? 1 : 0)) {
    // borrow 1 day
    ltm.hour += 24;
    borrow_date = 1;
  }
  
  ltm.hour -= clock_get_hour();
  ltm.hour -= (borrow_hour ? 1 : 0);
  
  return borrow_date;
}

static const unsigned char code leap_month[][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}};
// return day
static void lt_timer_sub_date(bit borrow_date)
{
  unsigned char i, j;
  int day;
  
  day = 0;
  
  // 减date
  for(i = clock_get_year();i < ltm.year; i ++)  
    day += clock_is_leap_year(i) ? 366 : 365;
  
  j = clock_is_leap_year(ltm.year) ? 1 : 0;
  
  for(i = 0 ; i < ltm.month; i ++)
    day += leap_month[j][i];
  
  for(i = 0 ; i < clock_get_month() - 1; i ++)
    day -= leap_month[j][i]; 
  
  day = day + ltm.date - clock_get_date() + 1;
 
  if(borrow_date)
    day --;
  
  if(day < 0) { // 在调用函数的时候，其实已经到时间了！
    ltm.date = 0;
    ltm.hour = 0;
    ltm.min = 0;
    ltm.sec = 0;
  } else {
    ltm.date = (unsigned char) day;
  }
}


/*
//////// add ltm and clk, 结果放在 year/month/date/hour/min/sec
// return min
static unsigned char lt_timer_add_sec_min(unsigned char * sec)
{
  // 加sec
  unsigned char min = 0;
  (*sec) = ltm.sec + clock_get_sec();
  if(*sec >= 60) {
    (*sec) = (*sec) % 60;
    min ++;
  }
  return min;
}

// return hour
static unsigned char lt_timer_add_min_hour(unsigned char * min)
{
  // 加min
  unsigned char hour = 0;
  (*min) += ltm.min + clock_get_min();
  if(*min >= 60) {
    (*min) = *min % 60;
    hour ++;
  }
  return hour;
}

// return date
static unsigned char lt_timer_add_hour_date(unsigned char * hour)
{
  // 加hour
  unsigned char date = 0;
  (*hour) += ltm.hour + clock_get_hour();
  if(*hour >= 24) {
    (*hour) = (*hour) % 24;
    date ++;
  }
  
  return date;
}

// return year
static unsigned char lt_timer_add_date_month_year(unsigned char * date, unsigned char * month)
{
  // 加date, month, year
  unsigned char year;
  (*date) += ltm.day + clock_get_date();
  (*month) = clock_get_month();
  year  = clock_get_year();
  while(*date >= clock_get_mon_date(year, (*month))) {
     CDBG("date = %bd mon = %bd year = %bd\n", (*date), (*month), year); 
    (*date) -= clock_get_mon_date(year, (*month));
    (*month) ++;
    if((*month) > 11) {
      (*month) = 0;
      year ++;
    }
  }
  
  return year;
}

*/

#pragma NOAREGS
void lt_timer_dec_sec(void)
{
  if(!lt_tmr_start)
    return;
  
  if(!lt_tmr_stopped) {
    if(ltm.sec != 0
      || ltm.min != 0
      || ltm.hour != 0) {
      ltm.sec --;
      if(ltm.sec == 255) {
        ltm.sec = 59;
        ltm.min --;
        if(ltm.min == 255){
          ltm.min = 59;
          ltm.hour --;
          if(ltm.hour == 255){
            ltm.hour = 23;
            if(ltm.date > 0) {
              ltm.date --;
            }
          }
        }
      }
    } else {
      lt_tmr_stopped = 1;
      set_task(EV_COUNTER);
    }
  }

  if(lt_tmr_display && !led_powersave) {
    if(lt_tmr_disp_day) {    
      led_data[5] = led_code[0x18]|0x80; // D
      led_data[4] = led_code[0x15]|0x80; // A
      led_data[3] = led_code[0x2D]|0x80; // Y
      led_data[2] = led_code[0x01]|0x80; // -
      led_data[1] = led_code[(ltm.date / 10) + 4]|0x80; 
      led_data[0] = led_code[(ltm.date % 10) + 4]|0x80; 
    } else {
      led_data[5] = led_code[(ltm.hour / 10) + 4]; 
      led_data[4] = led_code[(ltm.hour % 10) + 4];      
      led_data[3] = led_code[(ltm.min / 10) + 4]; 
      led_data[2] = led_code[(ltm.min % 10) + 4];    
      led_data[1] = led_code[(ltm.sec / 10) + 4]; 
      led_data[0] = led_code[(ltm.sec % 10) + 4]; 
      led_data[0] |= 0x80;
      led_data[5] |= 0x80;
    }  
  }
}
#pragma AREGS 

//////////////
// 从rom读出绝对时间,放入ltm
void lt_timer_sync_from_rom(void)
{
  ltm.year  = rom_read(ROM_LT_TIMER_YEAR);
  ltm.month = rom_read(ROM_LT_TIMER_MONTH);
  ltm.date  = rom_read(ROM_LT_TIMER_DATE);
  ltm.hour  = rom_read(ROM_LT_TIMER_HOUR);
  ltm.min   = rom_read(ROM_LT_TIMER_MIN);
  ltm.sec   = rom_read(ROM_LT_TIMER_SEC);
}

// 将绝对时间写入rom！
void lt_timer_sync_to_rom(enum lt_timer_sync_type type)
{
  CDBG("lt_timer_sync_to_rom type = %bd\n", type);
  switch (type) { 
    case LT_TIMER_SYNC_YEAR:
      rom_write(ROM_LT_TIMER_YEAR, ltm.year);
      break;
    case LT_TIMER_SYNC_MONTH:
      rom_write(ROM_LT_TIMER_MONTH, ltm.month);
      break;    
    case LT_TIMER_SYNC_DATE:
      rom_write(ROM_LT_TIMER_DATE, ltm.date);
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
// ltm绝对时间转换为相对时间，
// 如果too_close_check & 小于30S，返回0，否则返回1
// 如果!too_close_check，忽略30S逻辑
bit lt_timer_get_relative(bit too_close_check)
{
  bit borrow;
  
  // 计算相对时间，调用后ltm.month 没有意义了,ltm.year需要保留
  clock_enable_interrupt(0);
  borrow = lt_timer_sub_sec_min();
  borrow = lt_timer_sub_min_hour(borrow);
  borrow = lt_timer_sub_hour_date(borrow);
  lt_timer_sub_date(borrow);
  clock_enable_interrupt(1);
  
  CDBG("lt_timer_get_relative: ltm.date = %bd, ltm.min = %bd, ltm.sec = %bd\n",
    ltm.date, ltm.min, ltm.sec);
  
  // 所设置绝对时间已经超时了
  if(ltm.date == 0 && ltm.hour == 0 && ltm.min == 0) {
    if(ltm.sec == 0) {
      CDBG("lt_timer_get_relative : already triggered!\n");
      return 0;
    } else if(ltm.sec < LT_MIN_SEC && too_close_check) {
     CDBG("lt_timer_get_relative : too close!\n");
      return 0;
    } 
  }
  
  return 1;
}

// 将ltm里的绝对时间写入RTC
void lt_timer_sync_to_rtc(void)
{
  CDBG("lt_timer_sync_to_rtc : ltm.date = %bd, ltm.hour = %bd, ltm.min = %bd, ltm.sec = %bd\n",
  ltm.date, ltm.hour, ltm.min, ltm.sec);
  // 绝对时间写入rtc
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_mode(RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC);
  rtc_alarm_set_date(ltm.date + 1);
  rtc_alarm_set_hour(ltm.hour);
  rtc_alarm_set_min(ltm.min);
  rtc_alarm_set_sec(ltm.sec);
  rtc_write_data(RTC_TYPE_ALARM0);  
}

/*
// 将当前时间+ltm（相对时间），求出绝对时间写入rtc
// test current time + ltm > 2099-12-31-23:59:59
// 如果overflow，返回0，成功返回1
bit lt_timer_sync_to_rtc(void)
{
  bit ret = 1;
  //unsigned char temp0, temp1;
  unsigned char sec, min, hour, date, month, year;
  
  // about 280 ms!
  //temp0 = clock_get_ms39();
  clock_enable_interrupt(0);
  min = lt_timer_add_sec_min(&sec);
  hour = lt_timer_add_min_hour(&min);
  date = lt_timer_add_hour_date(&hour);
  year = lt_timer_add_date_month_year(&date, &month);
  clock_enable_interrupt(1);
  //temp1 = clock_get_ms39();

  //CDBG("temp0 = %bx temp1 = %bx\n", temp0, temp1);
  
  CDBG("lt_timer_is_overflow: [%02bd-%02bd-%02bd %02bd:%02bd:%02bd] + [%02bd %02bd:%02bd:%02bd] = [%02bd-%02bd-%02bd %02bd:%02bd:%02bd]\n", 
    clock_get_year(), clock_get_month(), clock_get_date(), clock_get_hour(), clock_get_min(), clock_get_sec(),
    ltm.day, ltm.hour, ltm.min, ltm.sec,
    year, month, date, hour, min, sec
  );  
  
  if(year > CLOCK_YEAR_BASE + 99) {
    ret = 0;
  } else if(year < CLOCK_YEAR_BASE + 99) {
    ret = 1;
  } else if(month > 11){
    ret = 0;
  } else if(month < 11) {
    ret = 1;
  } else if(date > 30) {
    ret = 0;
  } else if(date < 30) {
    ret = 1;
  } else if(hour > 59) {
    ret = 0;
  } else if(hour < 59) {
    ret = 1;
  } else if(min > 59) {
    ret = 0;
  } else if(min < 59) {
    ret = 1;
  } else if(sec > 59) {
    ret = 0;
  } else if(sec < 59) {
    ret = 1;
  } else {
    ret = 0;
  }
  
  if(!ret) {
    CDBG("lt_timer_is_overflow: is overflowed!\n");
    return ret;
  }
  
  ltm.ms39 = 0;
  
  lt_timer_start_ram();
  
  // 向rtc写入，但是不启动rtc
  rtc_read_data(RTC_TYPE_ALARM0);
  rtc_alarm_set_mode(RTC_ALARM0_MOD_MATCH_DATE_HOUR_MIN_SEC);
  rtc_alarm_set_date(date);
  rtc_alarm_set_hour(hour);
  rtc_alarm_set_min(min);
  rtc_alarm_set_sec(sec);
  rtc_write_data(RTC_TYPE_ALARM0);
  
  // year, month写入rom！,每次触发时候，需要看看当前year、mon是不是match
  rom_write(ROM_LT_TIMER_YEAR, year);
  rom_write(ROM_LT_TIMER_MONTH, month);
  
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
  ltm.day  = rom_read(ROM_LT_TIMER_DAY);
  ltm.hour = rom_read(ROM_LT_TIMER_HOUR);
  ltm.min  = rom_read(ROM_LT_TIMER_MIN);
  ltm.sec  = rom_read(ROM_LT_TIMER_SEC);
}
*/