#ifndef __CLOCK_DEBUG_H__
#define __CLOCK_DEBUG_H__

#include <stdio.h>

#define CLOCK_DEBUG

void debug_onoff(bit enable);
void debug_initialize(void);

#ifdef CLOCK_DEBUG
void debug_printf(const char * fmt, ...);
#define CDBG debug_printf
#else
#define CDBG
#endif

#endif
