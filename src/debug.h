#ifndef __CLOCK_DEBUG_H__
#define __CLOCK_DEBUG_H__

#include <stdio.h>

void debug_onoff(bit enable);
void debug_initialize(void);

#ifdef __CLOCK_DEBUG__
void debug_printf(const char * fmt, ...);
#define CDBG(exp) debug_printf exp
#else
#define CDBG(exp)
#endif

#endif
