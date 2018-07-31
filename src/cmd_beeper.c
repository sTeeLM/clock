#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "cmd_beeper.h"
#include "beeper.h"

#ifdef __CLOCK_DEBUG__

char cmd_beeper(char arg1, char arg2)
{
  if(arg1 != 0 && arg2 == 0 && strcmp("be", shell_buf + arg1) == 0) {
    beeper_beep();
  } else if(arg1 != 0 && arg2 == 0 && strcmp("bebe", shell_buf + arg1) == 0) {
    beeper_beep_beep();
  } else if(arg1 != 0 && arg2 == 0 && strcmp("on", shell_buf + arg1) == 0) {
    beeper_set_beep_enable(1);
    printf("beeper on\n");
  } else if(arg1 != 0 && arg2 == 0 && strcmp("off", shell_buf + arg1) == 0) {
    beeper_set_beep_enable(0);
    printf("beeper off\n");
  }else {
    return 1;
  }
  return 0;
}

#endif