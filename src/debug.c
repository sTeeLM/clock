#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

static bit debug_is_on;
  
void debug_initialize(void)
{
  debug_is_on = 1;
}

void debug_onoff(bit enable)
{
  debug_is_on = enable;
}

void debug_printf(const char * fmt, ...)
{
  va_list arg_ptr;
  
  if(debug_is_on) {
    va_start (arg_ptr, fmt); /* format string */
    vprintf (fmt, arg_ptr);
    va_end (arg_ptr);
  }
}
