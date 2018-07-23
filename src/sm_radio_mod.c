#include "sm_radio_mod.h"
#include "mod_common.h"
#include "radio.h"
#include "led.h"
#include "debug.h"
#include "cext.h"

const char * code sm_radio_mod_ss_name[] = {
  "SM_RADIO_MOD_INIT",
  "SM_RADIO_MOD_HLSI",
  "SM_RADIO_MOD_MS",
  "SM_RADIO_MOD_BL",
  "SM_RADIO_MOD_HCC",
  "SM_RADIO_MOD_SNC",
  "SM_RADIO_MOD_DTC",
  NULL
};

#define val common_state

static void show_on_off(unsigned char v)
{
  if(v) {
    led_set_code(2, LED_CODE_BLACK);
    led_set_code(1, 'O');
    led_set_code(0, 'N');
  } else {
    led_set_code(2, 'O');
    led_set_code(1, 'F');
    led_set_code(0, 'F');
  }
}

static void update_radio_mod(unsigned char what)
{
  led_clear();
  led_set_code(5, 'R');
  led_set_code(4, 'O');
  led_set_dp(4);
  led_set_dp(3);
  switch(what){
    case IS_RADIO_HLSI:
      led_set_code(3, '0');
      show_on_off(val);
    break;
    case IS_RADIO_MS:
      led_set_code(3, '1');
      show_on_off(val);
    break;
    case IS_RADIO_BL:
      led_set_code(3, '2');
      led_set_code(2, LED_CODE_BLACK);
      if(val == RADIO_BL_JAPNESE) {
        led_set_code(1, 'J');
        led_set_code(0, 'A');
      } else {
        led_set_code(1, 'E');
        led_set_code(0, 'U');
      }
    break;
    case IS_RADIO_HCC:
      led_set_code(3, '3');
      show_on_off(val);
    break;
    case IS_RADIO_SNC:
      led_set_code(3, '4');
      show_on_off(val);
    break;
    case IS_RADIO_DTC:
      led_set_code(3, '5');
      led_set_code(2, LED_CODE_BLACK);
      if(val == RADIO_DTC_75US) {
        led_set_code(1, '7');
        led_set_code(0, '5');
      } else {
        led_set_code(1, '5');
        led_set_code(0, '0');
      }
    break;
  }
}

static void enter_radio_mod(unsigned char what)
{
  switch(what){
    case IS_RADIO_HLSI:
      val = radio_get_hlsi();
    break;
    case IS_RADIO_MS:
      val = radio_get_ms();
    break;
    case IS_RADIO_BL:
      val = radio_get_bl();
    break;
    case IS_RADIO_HCC:
      val = radio_get_hcc();
    break;
    case IS_RADIO_SNC:
      val = radio_get_snc();
    break;
    case IS_RADIO_DTC:
      val = radio_get_dtc();
    break;
  }
  update_radio_mod(what);
}

static void toggle_radio_mod(unsigned char what)
{
  switch(what){
    case IS_RADIO_HLSI:
      val = radio_set_hlsi(!radio_get_hlsi());
      radio_write_rom_hlsi();
    break;
    case IS_RADIO_MS:
      val = radio_set_ms(!radio_get_ms());
      radio_write_rom_ms();
    break;
    case IS_RADIO_BL:
      val = radio_inc_bl();
      radio_write_rom_bl();
    break;
    case IS_RADIO_HCC:
      val = radio_set_hcc(!radio_get_hcc());
      radio_write_rom_hcc();
    break;
    case IS_RADIO_SNC:
      val = radio_set_snc(!radio_get_snc());
      radio_write_rom_snc();
    break;
    case IS_RADIO_DTC:
      val = radio_inc_dtc();
      radio_write_rom_dtc();
    break;
  }
  update_radio_mod(what);
}

static void sm_radio_mod(unsigned char what, enum task_events ev)
{
  if(ev == EV_KEY_MOD_PRESS) {
    enter_radio_mod(what);
  } else if(ev == EV_KEY_SET_PRESS) {
    toggle_radio_mod(what);
  }
}

void sm_radio_mod_init(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  UNUSED_PARAM(ev);
  
  display_logo(DISPLAY_LOGO_TYPE_RADIO, 1);
}

void sm_radio_mod_submod0(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);

  if(ev == EV_KEY_MOD_UP && get_sm_ss_state(from) == SM_RADIO_MOD_INIT) {
    ev = EV_KEY_MOD_PRESS;
  }
  sm_radio_mod(IS_RADIO_HLSI, ev);
}

void sm_radio_mod_submod1(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_radio_mod(IS_RADIO_MS, ev);
}

void sm_radio_mod_submod2(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_radio_mod(IS_RADIO_BL, ev);
}

void sm_radio_mod_submod3(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_radio_mod(IS_RADIO_HCC, ev);
}

void sm_radio_mod_submod4(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_radio_mod(IS_RADIO_SNC, ev);
}

void sm_radio_mod_submod5(unsigned char from, unsigned char to, enum task_events ev)
{
  UNUSED_PARAM(from);
  UNUSED_PARAM(to);
  
  sm_radio_mod(IS_RADIO_DTC, ev);
}