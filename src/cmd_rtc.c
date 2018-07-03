#include <string.h>
#include <stdlib.h>
#include "cmd_rtc.h"
#include "shell.h"
#include "debug.h"
#include "rtc.h"

char cmd_rtc(char arg1, char arg2)
{
  if(arg1 == 0 && arg2 == 0) {
    rtc_dump();
    return 0;
  }    
  return 1;
}