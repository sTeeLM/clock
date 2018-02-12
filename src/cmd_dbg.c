#include <string.h>
#include <stdlib.h>
#include "cmd_dbg.h"
#include "shell.h"
#include "debug.h"
#include "cext.h"

char cmd_dbg(char arg1, char arg2)
{
  
  UNUSED_PARAM(arg2);
  
  if(arg1 == 0)
    return 1;
  
  if(strcmp(shell_buf + arg1, "on") == 0) {
    printf("debug ON\n");
    debug_onoff(1);
  } else if(strcmp(shell_buf + arg1, "off") == 0) {
    printf("debug OFF\n");
    debug_onoff(0);
  } else {
    return 1;
  }
  
  return 0;
}