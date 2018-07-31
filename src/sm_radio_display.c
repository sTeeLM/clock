#include "sm_radio_display.h"
#include "power.h"
#include "radio.h"
#include "mod_common.h"
#include "power.h"
#include "led.h"
#include "debug.h"
#include "cext.h"
#include "sm_radio_powersave.h"

#define RADIO_SWITCH_TIMEO 7 // 每x秒在电台和音量之间轮换一次

#ifdef __CLOCK_DEBUG__
const char * code sm_radio_display_ss_name[] = 
{
  "SM_RADIO_DISPLAY_INIT",
  "SM_RADIO_DISPLAY_STATION",
  "SM_RADIO_DISPLAY_VOLUME",
  "SM_RADIO_DISPLAY_MOD",
  NULL
};
#endif

#define radio_display_lock lpress_lock_year_hour
#define switch_to common_state

static unsigned int val;

static void update_radio(unsigned char what)
{
  unsigned char tmp;
  led_clear();
  switch(what) {
    case IS_STATION:
    case IS_FREQUENCY:
      led_set_code(5, '-');
      tmp = (unsigned char)(val / 10);
      led_set_code(4, tmp / 100 + 0x30);
      led_set_code(3, (tmp % 100) / 10 + 0x30);
      led_set_code(2, (tmp % 100) % 10 + 0x30);
      tmp = (unsigned char)(val % 10);
      led_set_code(1, tmp + 0x30);
      led_set_code(0, '-');
      led_set_dp(2);
      if(radio_get_stereo()) {
        led_set_dp(5);
      } else {
        led_clr_dp(5);
      }
    break;
    case IS_VOLUME:
      led_set_code(5, '-');     
      led_set_code(4, '-');    
      led_set_code(3, val / 100 + 0x30);
      led_set_code(2, (val % 100) / 10 + 0x30);
      led_set_code(1, (val % 100) % 10 + 0x30);
      led_set_code(0, '-'); 
    break;
    case IS_RADIO_MOD:
      led_set_code(5, '-');     
      led_set_code(4, '-');
      led_set_code(3, LED_CODE_CEL);
      led_set_code(2, LED_CODE_CEL);
      led_set_code(1, '-');
      led_set_code(0, '-'); 
    break;
  }
}

static void radio_freq_cb(unsigned int freq)
{
  unsigned char tmp;
  tmp = (unsigned char)(freq / 10);
  led_set_code(4, tmp / 100 + 0x30);
  led_set_code(3, (tmp % 100) / 10 + 0x30);
  led_set_code(2, (tmp % 100) % 10 + 0x30);
  tmp = (unsigned char)(freq % 10);
  led_set_code(1, tmp + 0x30);
  if(radio_get_stereo()) {
    led_set_dp(5);
  } else {
    led_clr_dp(5);
  }
}

static void enter_radio(unsigned char what)
{
  radio_display_lock = 0;
  switch(what) {
    case IS_STATION:
    case IS_FREQUENCY:
      val = radio_get_frequency();
    break;
    case IS_VOLUME:
      val = radio_get_volume();
    break;
    case IS_RADIO_MOD:
    break;
  }
  update_radio(what);
}

static void dec_only(unsigned char what)
{
  if(radio_display_lock == 0) {
    radio_display_lock = 1;
  }
  switch(what) {
    case IS_STATION:
      val = radio_prev_station(radio_freq_cb);
      break;
    case IS_FREQUENCY:
      val = radio_dec_frequency();
      break;
    case IS_VOLUME:
      val = radio_dec_volume();
      break;
    case IS_RADIO_MOD:
      break;
  }
  update_radio(what);
}

static void inc_only(unsigned char what)
{
  if(radio_display_lock == 0) {
    radio_display_lock = 1;
  }
  switch(what) {
    case IS_STATION:
      val = radio_next_station(radio_freq_cb);
      break;
    case IS_FREQUENCY:
      val = radio_inc_frequency();
      break;
    case IS_VOLUME:
      val = radio_inc_volume();
      break;
    case IS_RADIO_MOD:
      break;
  }
  update_radio(what);
}

static void write_only(unsigned char what)
{
  if(radio_display_lock == 1) {
    switch(what) {
      case IS_STATION:
      case IS_FREQUENCY:
        radio_write_rom_frequency();
        break;
      case IS_VOLUME:
        radio_write_rom_volume();
        break;
    case IS_RADIO_MOD:
      break;
    }
    radio_display_lock = 0;
  }
}

static void dec_write(unsigned char what)
{
  dec_only(what);
  write_only(what);
}

static void inc_write(unsigned char what)
{
  inc_only(what);
  write_only(what);
}

static void reset_switch_to(void)
{
  switch_to = 0;
}

static void test_switch_to(void)
{
  if(radio_display_lock != 1) {
    switch_to ++;
  }
  if(switch_to > RADIO_SWITCH_TIMEO) {
    switch_to = 0;
    set_task(EV_KEY_V0);
  }
}

void sm_radio_display_init(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
  display_logo(DISPLAY_LOGO_TYPE_RADIO, 0);
  power_5v_enable(0);
  radio_enable(1);
  power_reset_powersave_to();
}

void sm_radio_display_submod0(unsigned char from, unsigned char to, enum task_events ev)
{

  UNUSED_PARAM(to);

  if((ev == EV_KEY_MOD_UP && get_sm_ss_state(from) == SM_RADIO_DISPLAY_INIT) || ev == EV_KEY_V0) {
    enter_radio(IS_STATION);
    reset_switch_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_PRESS) {
    if(from != (SM_RADIO_POWERSAVE<<4|SM_RADIO_POWERSAVE_SLEEP)) {
      dec_write(IS_STATION);
    } else {
      enter_radio(IS_STATION);
    }
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      dec_only(IS_FREQUENCY);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_UP) {
    write_only(IS_FREQUENCY);
    lpress_start = 0;
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    if(from != (SM_RADIO_POWERSAVE<<4|SM_RADIO_POWERSAVE_SLEEP)) {
      inc_write(IS_STATION);
    } else {
      enter_radio(IS_STATION);
    }    
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_FREQUENCY);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_FREQUENCY);
    lpress_start = 0;
    return;
  }
  
  if(ev == EV_1S) {
    test_switch_to();
    power_test_powersave_to();
    return;
  }
}

void sm_radio_display_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(ev == EV_KEY_V0) {
    enter_radio(IS_VOLUME);
    reset_switch_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_PRESS) {
    dec_write(IS_VOLUME);
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      dec_only(IS_VOLUME);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_MOD_UP) {
    write_only(IS_VOLUME);
    lpress_start = 0;
    return;
  }
  
  if(ev == EV_KEY_SET_PRESS) {
    inc_write(IS_VOLUME);
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_SET_LPRESS) {
    if((lpress_start % LPRESS_INC_DELAY) == 0) {
      inc_only(IS_VOLUME);
    }
    lpress_start++;
    if(lpress_start == LPRESS_INC_OVERFLOW) lpress_start = 0;
    reset_switch_to();
    power_reset_powersave_to();
    return;
  }
  
  if(ev == EV_KEY_SET_UP) {
    write_only(IS_VOLUME);
    lpress_start = 0;
    return;
  }
  
  if(ev == EV_1S) {
    test_switch_to();
    power_test_powersave_to();
    return;
  }
}

void sm_radio_display_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  if(ev == EV_KEY_V0) {
    enter_radio(IS_RADIO_MOD);
    reset_switch_to();
    return;
  }
  
  if(ev == EV_1S) {
    test_switch_to();
    power_test_powersave_to();
    return;
  }
}
