#ifndef __CLOCK_DEBUG_H__
#define __CLOCK_DEBUG_H__

#define CLOCK_DEBUG

#ifdef CLOCK_DEBUG
#include <stdio.h>
#define CDBG printf
#else
#define CDBG
#endif

#endif