#include <STC89C5xRC.H>

#include <string.h>
#include <stdio.h>
#include "cmd_reboot.h"
#include "shell.h"
#include "power.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__

char cmd_reboot(char arg1, char arg2)
{
  SHELL_CMD_PROC fuck;
  
  UNUSED_PARAM(arg2);
  
  if(arg1 == 0) {
#ifdef __CLOCK_EMULATE__    
    fuck = NULL;
    fuck(0,0);
#else
    UNUSED_PARAM(fuck);
    ISP_CONTR |= 0x20;
#endif
    return 0;
  } else if(strcmp(shell_buf + arg1, "off") == 0) {
    power_3_3v_enable(0);
    return 0;
  } else if(strcmp(shell_buf + arg1, "isp") == 0) {
#ifdef __CLOCK_EMULATE__ 
    printf ("reboot to isp was not supported\n");
    return 1;
#else
    ISP_CONTR |= 0x60;
    return 0;
#endif
  }
  return 1;
}

#endif