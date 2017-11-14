#include "sm_fuse_grenade.h"
#include "debug.h"
#include "mod_common.h"
#include "rom.h"
#include "led.h"
#include "gyro.h"
#include "fuse.h"

const char * code sm_fuse_grenade_ss_name[] = 
{
  "SM_FUSE_GRENADE_INIT",
  "SM_FUSE_GRENADE_PREARMED",
  "SM_FUSE_GRENADE_ARMED",
  "SM_FUSE_GRENADE_DISARMED",
  "SM_FUSE_GRENADE_PREDETONATE",
  NULL
};

static bit grenade_check_set_param(void)
{
  unsigned char val;
  // gyro必须在配置里打开
  val = rom_read(ROM_FUSE_GYRO_ONOFF);
  if(!val) {
    display_param_error(PARAM_ERROR_NEED_GYRO);
    return 0;
  }
  
  // gyro 必须是好的
  val = rom_read(ROM_GYRO_GOOD);
  if(!val) {
    display_param_error(PARAM_ERROR_GYRO_BAD);
    return 0;
  }
  
  gyro_enable(1);
  
  if(rom_read(ROM_FUSE0_SHORT_GOOD) 
    &&rom_read(ROM_FUSE1_SHORT_GOOD)
    &&rom_read(ROM_FUSE0_BROKE_GOOD)
    &&rom_read(ROM_FUSE1_BROKE_GOOD)
  ) {
    fuse_enable(1);
  } else {
    display_param_error(PARAM_ERROR_FUSE_ERROR);
    gyro_enable(0);
    return 0;
  }
  return 1;
}


enum grenade_display_state
{
  DISPLAY_GRENADE_PREARMED,
  DISPLAY_GRENADE_DISARMED,
  DISPLAY_GRENADE_ARMED,
  DISPLAY_GRENADE_PREDETONATE,
};

static void display_grenade(enum grenade_display_state state)
{
  led_clear();
  led_set_code(5, 'P');
  led_set_code(4, 'L');
  led_set_code(3, '1');
  led_set_code(2, '-');
  switch(state) {
    case DISPLAY_GRENADE_PREARMED:
      led_set_code(0, 'A');
      led_set_code(1, 'A');
      led_set_blink(0);
      led_set_blink(1);    
      break;
    case DISPLAY_GRENADE_DISARMED:
      led_set_code(0, 'D');
      led_set_code(1, 'D');
      break;    
    case DISPLAY_GRENADE_ARMED:
      led_set_code(0, 'A');
      led_set_code(1, 'A');
      break;
    case DISPLAY_GRENADE_PREDETONATE:
      led_set_code(0, 'C');
      led_set_code(1, 'C');
      break;
  }
}

void sm_fuse_grenade(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_grenade %bd %bd %bd\n", from, to, ev);
  
  // 从别的状态切过来
  if(get_sm_ss_state(to) == SM_FUSE_GRENADE_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 4);
    return;
  }
  
  // 过1S进入prearm状态，设置参数，不成功发送EV_FUSE_SEL0
  if(get_sm_ss_state(from) == SM_FUSE_GRENADE_INIT
    && get_sm_ss_state(to) == SM_FUSE_GRENADE_PREARMED && ev == EV_1S) {
    display_grenade(DISPLAY_GRENADE_PREARMED);
    if(!grenade_check_set_param()) {
      set_task(EV_FUSE_SEL0);
    }
    return;
  }
  
  // 解除
  if(get_sm_ss_state(from) == SM_FUSE_GRENADE_PREARMED
    && get_sm_ss_state(to) == SM_FUSE_GRENADE_DISARMED && ev == EV_KEY_MOD_LPRESS) {
    // 清理，关闭fuse
    display_grenade(DISPLAY_GRENADE_DISARMED);
    gyro_enable(0);
    fuse_enable(0);
    set_task(EV_FUSE_SEL0);
    return;
  }
    
  // 脱手
  if(get_sm_ss_state(from) == SM_FUSE_GRENADE_PREARMED
    && get_sm_ss_state(to) == SM_FUSE_GRENADE_ARMED && ev == EV_DROP_GYRO) {
    display_grenade(DISPLAY_GRENADE_ARMED);
    return;
  }
    
  // 触碰
  if(get_sm_ss_state(from) == SM_FUSE_GRENADE_ARMED
    && get_sm_ss_state(to) == SM_FUSE_GRENADE_ARMED && ev == EV_ACC_GYRO) {
    display_grenade(DISPLAY_GRENADE_ARMED);
    set_task(EV_FUSE_SEL0);
    return;
  }
    
  // detonate前清理
  if(get_sm_ss_state(from) == SM_FUSE_GRENADE_ARMED
    && get_sm_ss_state(to) == SM_FUSE_GRENADE_PREDETONATE && ev == EV_FUSE_SEL0) {
    display_grenade(DISPLAY_GRENADE_PREDETONATE);
    // 清理，不关fuse
    gyro_enable(0);
    set_task(EV_FUSE_SEL0);
    return;
  }
}