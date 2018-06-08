#include "cmd_help.h"
#include "shell.h"
#include "debug.h"
#include "cext.h"

char cmd_help(char arg1, char arg2)
{
  char i; 

  UNUSED_PARAM(arg2);
  
  if(arg1 == 0) {
    shell_dump_cmd();
  } else {
    i = shell_search_cmd_by_name(shell_buf + arg1);
    if(i != -1) {
      printf("usage:\n%s\n", cmds[i].usage);
    } else {
      printf("unknown cmd '%s'\n",shell_buf + arg1);
    }
  }
  return 0;
}
