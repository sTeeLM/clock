#include "sm_fuse_grenade.h"
#include "debug.h"
#include "mod_common.h"
#include "rom.h"

/*
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
*/
void sm_fuse_grenade(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_grenade %bd %bd %bd\n", from, to, ev);
  if(get_sm_ss_state(to) == SM_FUSE_GRENADE_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 4);
    return;
  }
}