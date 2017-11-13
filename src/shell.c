#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "shell.h"

#include "cmd_help.h"
#include "cmd_serial.h"
#include "cmd_int.h"
#include "cmd_i2c.h"
#include "cext.h"

#define SHELL_BUFFER_SIZE 21

char shell_buf[SHELL_BUFFER_SIZE];

static char cmd_null(char arg1, char arg2)
{
  UNUSED_PARAM(arg1);
  UNUSED_PARAM(arg2);
  return 0;
}

struct shell_cmds code cmds[] = 
{
  {"?",  "show help", "?: list cmd\n? <cmd>: show usage of cmd", cmd_help},
  {"sp", "set or get serial hub", "sp <num> <0|1>: set port #num to 0/1\nsp <num>: show port value of #num", cmd_serial},
  {"ip", "get status of int hub", "ip: dump status of ip port\nip 1: dump reg of ext int", cmd_int},
  {"ir", "read data from i2c", "ir <addr> <cmd>: read one byte", cmd_i2c},
  {"iw", "write data to i2c", "iw <addr>: set addr\niw <cmd> <data>: write one byte", cmd_i2c},  
  {"ex", "quit the shell", "ex", cmd_null}
}; 


char shell_search_cmd_by_name(char * cmd)
{
  char i;
  for (i = 0 ; i < sizeof(cmds)/sizeof(struct shell_cmds) ; i ++) {
    if(strcmp(cmd, cmds[i].cmd) == 0) {
      return i;
    }
  }
  return -1;
}

static void call_cmd(char * buf, char arg1, char arg2)
{
  char i;
  
  i = shell_search_cmd_by_name(buf);
  if(i != -1) {
    if(cmds[i].proc(arg1, arg2) != 0) { // C212
      printf("%s:\n%s\n", cmds[i].cmd, cmds[i].usage);
    }
  } else {
    printf("unknown cmd %s\n");
  }
}

void shell_dump_cmd(void)
{
  char i;
  for (i = 0 ; i < sizeof(cmds)/sizeof(struct shell_cmds) ; i ++) {
    printf("%s: %s\n", cmds[i].cmd, cmds[i].desc);
  }
}

void run_shell(void)
{
  char arg1_pos, arg2_pos;
  
  printf("++++++++++++++++++++++++++++++++++++++++\n");
  printf("+             tini SHELL               +\n");
  printf("++++++++++++++++++++++++++++++++++++++++\n");
  
  do {
    printf("shell>");
    
    gets (shell_buf, sizeof(shell_buf)-1);
    
    shell_buf[sizeof(shell_buf)-1] = 0;
    
    arg1_pos = 0;
    arg2_pos = 0;
    
    while(arg1_pos < sizeof(shell_buf) && shell_buf[arg1_pos] != ' ') arg1_pos ++;
    
    if(arg1_pos >= sizeof(shell_buf)) {
      call_cmd(shell_buf, 0, 0);
      continue;
    }
    
    while(arg1_pos < sizeof(shell_buf) && shell_buf[arg1_pos] == ' ') {
      shell_buf[arg1_pos] = 0;
      arg1_pos ++;
    }
    
    if(arg1_pos >= sizeof(shell_buf)) {
      call_cmd(shell_buf, 0, 0);
      continue;
    }
    
    arg2_pos = arg1_pos;
    
    while(arg2_pos < sizeof(shell_buf) && shell_buf[arg2_pos] != ' ') arg2_pos ++;
    
    if(arg2_pos >= sizeof(shell_buf)) {
      call_cmd(shell_buf, arg1_pos, 0);
      continue;
    }
    
    while(arg2_pos < sizeof(shell_buf) && shell_buf[arg2_pos] == ' ') {
      shell_buf[arg2_pos] = 0;
      arg2_pos ++;
    }
    
    if(arg2_pos >= sizeof(shell_buf)) {
      call_cmd(shell_buf, arg1_pos, 0);
      continue;
    }
    
    call_cmd(shell_buf, arg1_pos, arg2_pos);
    
  } while (strcmp(shell_buf, "ex") != 0);
  
}