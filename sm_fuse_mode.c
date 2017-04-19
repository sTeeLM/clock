#include "sm_fuse_mode.h"
#include "debug.h"

void sm_fuse_mode(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_mode %bd %bd %bd\n", from, to, ev);
}