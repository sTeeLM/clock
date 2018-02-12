#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmd_sm.h"
#include "shell.h"
#include "sm.h"

char cmd_sm(char arg1, char arg2)
{
  if(arg1 == 0) { // show current table, state, sub-state
      sm_show_current();
  } else {
    if(strcmp(shell_buf + arg1, "ll") == 0) { // list name of all tables, states, sub-states
      sm_dump();
    } else if(strcmp(shell_buf + arg1, "lt") == 0 && arg2 == 0) { // list tables
      sm_dump_table();
    } else if(strcmp(shell_buf + arg1, "st") == 0 && arg2 != 0) { // set table
      if(sm_set_table_by_name(shell_buf + arg2) != 0) {
       printf("unknown table name %s\n", shell_buf + arg2); 
      }
    } else if(strcmp(shell_buf + arg1, "ls") == 0 && arg2 == 0) { // list state
      sm_dump_state();
    } else if(strcmp(shell_buf + arg1, "ss") == 0 && arg2 != 0) { // set state
      if(sm_set_state_by_name(shell_buf + arg2) != 0) {
       printf("unknown state name %s\n", shell_buf + arg2); 
      }
    } else if(strcmp(shell_buf + arg1, "lss") == 0 && arg2 == 0) { // list sub-state
      sm_dump_sub_state();
    } else if(strcmp(shell_buf + arg1, "sss") == 0 && arg2 != 0) { // set sub-state
      if(sm_set_sub_state_by_name(shell_buf + arg2) != 0) {
       printf("unknown state name %s\n", shell_buf + arg2); 
      }
    } else {
      return 1;
    }
  }
  return 0;
}
