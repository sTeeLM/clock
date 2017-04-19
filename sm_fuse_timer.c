#include "sm_fuse_timer.h"
#include "debug.h"

void sm_fuse_timer(unsigned char from, unsigned char to, enum task_events ev)
{
  CDBG("sm_fuse_timer %bd %bd %bd\n", from, to, ev);
}