#include "sm_fuse_grenade.h"
#include "debug.h"
#include "mod_common.h"

void sm_fuse_grenade(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_grenade %bd %bd %bd\n", from, to, ev);
  if(get_sm_ss_state(to) == SM_FUSE_GRENADE_INIT) {
    display_logo(DISPLAY_LOGO_TYPE_FUSE, 4);
    return;
  }
}