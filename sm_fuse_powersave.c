#include "sm_fuse_powersave.h"
#include "debug.h"

void sm_fuse_powersave(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_powersave %bd %bd %bd\n", from, to, ev);
}