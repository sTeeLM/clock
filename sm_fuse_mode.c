#include "sm_fuse_mode.h"
#include "debug.h"
#include "mod_common.h"
#include "led.h"
#include "rom.h"
#include "lt_timer.h"
#include "thermo.h"

#define FUSE_MODE_DISPLAY_TIMER   0
#define FUSE_MODE_DISPLAY_GERNADE 1

enum param_error
{
  PARAM_ERROR_NEED_GYRO = 501,
  PARAM_ERROR_GYRO_BAD  = 502,
  PARAM_ERROR_LT_TIMER_OVERFLOW = 503,
  PARAM_ERROR_HG_BAD = 504,
  PARAM_ERROR_TRIPWIRE_BAD = 505,
  PARAM_ERROR_THERMO_HI_BAD = 506,
  PARAM_ERROR_THERMO_TOO_HI = 507,
  PARAM_ERROR_THERMO_LO_BAD = 508,
  PARAM_ERROR_THERMO_TOO_LOW = 508,
  PARAM_ERROR_THERMO_HI_LESS_LO = 510
};

static void display_mode(unsigned char mode)
{
  led_clear();
  led_set_code(5, 'P');
	led_set_code(4, 'L');
  switch(mode) {
    case FUSE_MODE_DISPLAY_TIMER:
      led_set_code(3, '0');
      break;
    case FUSE_MODE_DISPLAY_GERNADE:
      led_set_code(3, '1');
      break;
  }
}

#define PARAM_ERROR_NEED_GYRO       501
#define PARAM_ERROR_LT_TIMER_TIMERO 502

static void display_error(unsigned int err)
{
  led_clear();
  led_set_code(5, 'E');
  led_set_code(4, 'R');
  led_set_code(3, 'R');		
  led_set_code(2, (err / 100) + 0x30);
  led_set_code(1, ((err % 100)/10) + 0x30);  
  led_set_code(0, (err % 10) + 0x30); 
}

static bit check_grenade_param(void)
{
  unsigned char val;
  // gyro必须在配置里打开
  val = rom_read(ROM_FUSE_GYRO_ONOFF);
  if(!val) {
    display_error(PARAM_ERROR_NEED_GYRO);
    return 0;
  }
  
  // gyro 必须是好的
  val = rom_read(ROM_GYRO_GOOD);
  if(!val) {
    display_error(PARAM_ERROR_GYRO_BAD);
    return 0;
  }
  
  return 1;
}

static bit check_timer_param(void)
{
  unsigned char val, thermo_hi, thermo_lo;
  char current_temp;
  
  thermo_hi_enable(1);
  current_temp = thermo_ger_current();
  thermo_hi_enable(0);
  
  
  // lt_timer时间必须在当前时间之后
  lt_timer_sync_from_rom();
  if(lt_timer_is_overflow()) {
    display_error(PARAM_ERROR_LT_TIMER_OVERFLOW);
    return 0;
  }
  
  // 如果打开了gyro，必须是好的
  val = rom_read(ROM_FUSE_GYRO_ONOFF);
  if(val) {
    val = rom_read(ROM_GYRO_GOOD);
    if(!val) {
      display_error(PARAM_ERROR_GYRO_BAD);
      return 0;
    }
  }
  
  // 如果打开了hg，必须是好的
  val = rom_read(ROM_FUSE_HG_ONOFF);
  if(val) {
    val = rom_read(ROM_HG_GOOD);
    if(!val) {
      display_error(PARAM_ERROR_HG_BAD);
      return 0;
    }
  }
  
  // 如果打开了tripwire，必须是好的
  val = rom_read(ROM_FUSE_TRIPWIRE_ONOFF);
  if(val) {
    val = rom_read(ROM_TRIPWIRE_GOOD);
    if(!val) {
      display_error(PARAM_ERROR_TRIPWIRE_BAD);
      return 0;
    }
  }  
  
  // 如果打开了thermo hi，必须是好的
  thermo_hi = rom_read(ROM_FUSE_THERMO_HI);
  if(thermo_hi != THERMO_THRESHOLED_INVALID) {
    val = rom_read(ROM_THERMO_HI_GOOD);
    if(!val) {
      display_error(PARAM_ERROR_THERMO_HI_BAD);
      return 0;
    }
    if(current_temp >= (char)thermo_hi) {
      display_error(PARAM_ERROR_THERMO_TOO_HI);
      return 0;
    }
  }
  
  // 如果打开了thermo lo，必须是好的
  thermo_lo = rom_read(ROM_FUSE_THERMO_LO);
  if(thermo_lo != THERMO_THRESHOLED_INVALID) {
    val = rom_read(ROM_THERMO_LO_GOOD);
    if(!val) {
      display_error(PARAM_ERROR_THERMO_LO_BAD);
      return 0;
    }
    if(current_temp <= (char)thermo_lo) {
      display_error(PARAM_ERROR_THERMO_TOO_LOW);
      return 0;
    }
  }
  
  // 温度上下限不能倒置
  if(thermo_hi != THERMO_THRESHOLED_INVALID 
    && thermo_lo != THERMO_THRESHOLED_INVALID)
  {
    if((char)thermo_hi <= (char)thermo_lo) {
      display_error(PARAM_ERROR_THERMO_HI_LESS_LO);
      return 0;
    }
  }
  
  return 1;
}

void sm_fuse_mode(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_mode %bd %bd %bd\n", from, to, ev);

  // 按mod1进入模式选择
  if(get_sm_ss_state(to) == SM_FUSE_MODE_INIT && ev == EV_KEY_MOD_LPRESS) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 2);
    return;
  }
  
  // 从别的状态切换过来，防止误操作
  if(get_sm_ss_state(from) == SM_FUSE_MODE_INIT
    && get_sm_ss_state(to) == SM_FUSE_MODE_TIMER) {
    display_mode(FUSE_MODE_DISPLAY_TIMER);
    return;
  }
    
  // mod0 切换触碰模式
  if(get_sm_ss_state(to) == SM_FUSE_MODE_TIMER && (ev == EV_KEY_MOD_PRESS || ev == EV_1S)) {
    display_mode(FUSE_MODE_DISPLAY_TIMER);
    return;
  }  
  // set0 选择
  if(get_sm_ss_state(to) == SM_FUSE_MODE_TIMER && ev == EV_KEY_SET_PRESS) {
    if(check_timer_param()) {
      set_task(EV_FUSE_SEL0);
    }
    return;
  }
  
  // mod0 切换触碰模式
  if(get_sm_ss_state(to) == SM_FUSE_MODE_GRENADE && (ev == EV_KEY_MOD_PRESS || ev == EV_1S)) {
    display_mode(FUSE_MODE_DISPLAY_GERNADE);
    return;
  }
  
  // set0 选择
  if(get_sm_ss_state(to) == SM_FUSE_MODE_GRENADE && ev == EV_KEY_SET_PRESS) {
    if(check_grenade_param()) {
      set_task(EV_FUSE_SEL0);
    }
    return;
  }
}