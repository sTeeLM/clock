#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "cmd_beeper.h"
#include "beeper.h"

char cmd_beeper(char arg1, char arg2)
{
  char index;
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
  } else if(arg1 != 0 && arg2 == 0 && strcmp("next", shell_buf + arg1) == 0) {
    index = beeper_get_music_index();
    beeper_inc_music_index();
    printf("set music index : %bd -> %bd\n", index, beeper_get_music_index());
  } else if(arg1 != 0 && arg2 == 0 && strcmp("music", shell_buf + arg1) == 0) {
    printf("press MOD or SET key to stop music\n");
    beeper_play_music();
  }else {
    return 1;
  }
  return 0;
}
