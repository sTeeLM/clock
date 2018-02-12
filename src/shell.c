#include <stdio.h>
#include <string.h>
#include "debug.h"
#include "shell.h"

#include "led.h"
#include "clock.h"

#include "cmd_help.h"
#include "cmd_serial.h"
#include "cmd_int.h"
#include "cmd_i2c.h"
#include "cmd_dbg.h"
#include "cmd_reboot.h"
#include "cmd_task.h"
#include "cmd_sm.h"
#include "cmd_beeper.h"
#include "cext.h"

#define SHELL_BUFFER_SIZE 41

char shell_buf[SHELL_BUFFER_SIZE];

static char cmd_null(char arg1, char arg2)
{
  UNUSED_PARAM(arg1);
  UNUSED_PARAM(arg2);
  printf("quit!\n");
  return 0;
}

struct shell_cmds code cmds[] = 
{
  {"?",  "show help", "?: list cmd\n"
                      "? <cmd>: show usage of cmd", cmd_help},
  {"db", "debug on/off", "db <on|off>: switch debug message on/off", cmd_dbg},
  {"sp", "set or get serial hub", "sp <num> <0|1>: set port #num to 0/1\n"
                                  "sp <num>: show port value of #num", cmd_serial},
  {"ip", "get status of int hub", "ip: dump status of ip port\n"
                                  "ip ext: dump reg of ext int", cmd_int},
  {"ir", "read data from i2c", "ir <addr> <cmd>: read one byte", cmd_i2c},
  {"iw", "write data to i2c", "iw <addr>: set addr\n"
                              "iw <cmd> <data>: write one byte", cmd_i2c}, 
  {"rb", "reboot", "rb: reboot to user code (ap)\n"
                   "rb isp: reboot to isp", cmd_reboot},
  {"tsk", "task managment", "tsk: list all task state\n"
                            "tsk <task index> <1|0>: trigger task or clear task", cmd_task},
  {"sm", "state machine managment", "sm: show current state\n"
                                    "sm ll: list name of all tables, states, sub-states\n"
                                    "sm lt: list tables\n"
                                    "sm st <table-name>: set current table\n"
                                    "sm ls: list states\n"
                                    "sm ss <state-name>: set current state\n"
                                    "sm lss: list sub-state\n"
                                    "sm sss <sub-state-name>: set sub-state", cmd_sm},
  {"bp", "beeper test", "bp <on|off>: enable/disable\n"
                        "bp be: 'be'\n"
                        "bp bebe: 'bebe'\n"
                        "bp next: next music\n"
                        "bp music: play music", cmd_beeper},
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
  printf("buffer is '%s'\n", buf);
  i = shell_search_cmd_by_name(buf);
  if(i != -1) {
    if(cmds[i].proc(arg1, arg2) != 0) { // C212
      printf("%s:\n%s\n", cmds[i].cmd, cmds[i].usage);
    }
  } else {
    printf("unknown cmd '%s'\n", buf);
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
  
  // stop the clock
  clock_enter_shell();
  led_clear();
  led_set_code(5, 'S');
  led_set_code(4, 'H'); 
  led_set_code(3, 'E');  
  led_set_code(2, 'L'); 
  led_set_code(1, 'L');  
  do {
    printf("shell>");
    
    gets (shell_buf, sizeof(shell_buf)-1);
    
    shell_buf[sizeof(shell_buf)-1] = 0;
    
    if(shell_buf[0] == 0)
      continue;
    
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
    
    if(arg1_pos >= sizeof(shell_buf) || shell_buf[arg1_pos] == 0) {
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
    
    if(arg2_pos >= sizeof(shell_buf) || shell_buf[arg2_pos] == 0) {
      call_cmd(shell_buf, arg1_pos, 0);
      continue;
    }
    
    call_cmd(shell_buf, arg1_pos, arg2_pos);
    
  } while (strcmp(shell_buf, "ex") != 0);
  
  led_clear();
  clock_leave_shell();  
}