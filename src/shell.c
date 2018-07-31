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
#include "cmd_thermo.h"
#include "cmd_mpu.h"
#include "cmd_rtc.h"
#include "cmd_power.h"
#include "cmd_radio.h"
#include "cext.h"

#ifdef __CLOCK_DEBUG__

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
                   "rb off: power off\n"
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
                        "bp bebe: 'bebe'\n", cmd_beeper},
  {"th", "thermo test", "th : get current temperature\n"
                        "th hi <on|off> : enable/disable hi\n"
                        "th hi: get hi threshold\n"
                        "th hi inc: inc hi threshold\n"
                        "th hi dec: dec hi threshold\n"
                        "th hi <num>: set hi threshold to num\n"
                        "th hi rst: reset hi threshold\n"   
                        "th lo: get hi threshold\n"
                        "th lo <on|off> : enable/disable lo\n"
                        "th lo inc: inc hi threshold\n"
                        "th lo dec: dec hi threshold\n"
                        "th lo <num>: set lo threshold to num\n"
                        "th lo rst: reset lo threshold\n", cmd_thermo},
  {"mp", "mpu test",    "mp <on|off>: enable/disable\n"
                        "mp int: read interrupt status\n", cmd_mpu},
  {"rt", "rtc read or write",    "rt : dump status\n", cmd_rtc},  
  {"pw", "power test",         "pw bat: get current battery volume\n"
                               "pw 5v <on | off>: 5v power on off\n"
                               "pw hi <num>: hi alert voltage\n"
                               "pw lo <num>: lo alert voltage\n"
                               "pw hyst <num>: hysteresis voltage\n"
                               "pw int <clr>: alert status or clear alert\n" 
                               "pw cal <on | off>: alert status or clear alert\n"    
                              ,cmd_power}, 
  {"ra", "radio test",         "ra <on|off>: power on | off radio\n"
                               "ra station <prev|next|dec|inc|num> prev /next station\n"
                               "ra vol <num>: set volume\n"
                               "ra hlsi <hi|lo>: high/low side injection hi|lo\n"
                               "ra ms <on|off>: mono to stereo on |off \n" 
                               "ra bl <ja | eu>: band limits is Japanese FM(76~91) or US/Europe FM(87.5~108)\n" 
                               "ra hcc <on|off>: high cut control on|off \n" 
                               "ra snc <on|off>: stereo noise cancelling on|off \n"  
                               "ra dtc <75|50>: de-emphasis time constant is 75 μs or 50 us \n" 
                              ,cmd_radio},
  {"ex", "quit the shell", "ex", cmd_null},
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

void shell_dump_cmd(void)
{
  char i;
  for (i = 0 ; i < sizeof(cmds)/sizeof(struct shell_cmds) ; i ++) {
    printf("%s: %s\n", cmds[i].cmd, cmds[i].desc);
  }
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
    printf("unknown cmd '%s'\n", buf);
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
  led_save();
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
  led_restore();
  clock_leave_shell();  
}

#endif
