#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "cmd_thermo.h"
#include "thermo.h"

char cmd_thermo(char arg1, char arg2)
{
  char val;
  if(arg1 == 0) { // get current temperature
    val = thermo_get_current();
    printf("thermo current: %bd\n", val);
  } else if(arg2 == 0){
    if(!strcmp(shell_buf + arg1, "hi")) {
      val = thermo_hi_threshold_get();
      printf("thermo hi threshold: %bd\n", val);
    } else if(!strcmp(shell_buf + arg1, "lo")) {
      val = thermo_lo_threshold_get();
      printf("thermo lo threshold: %bd\n", val);
    } else {
      return 1;
    }
  } else {
    if(!strcmp(shell_buf + arg1, "hi")) {
      if(!strcmp(shell_buf + arg2, "inc")) {
        thermo_hi_threshold_inc();
      } else if(!strcmp(shell_buf + arg2, "dec")) {
        thermo_hi_threshold_dec();
      } else if(!strcmp(shell_buf + arg2, "rst")) {
        thermo_hi_threshold_reset();
      } else if(!strcmp(shell_buf + arg2, "on")) {
        thermo_hi_enable(1);
      } else if(!strcmp(shell_buf + arg2, "off")) {
        thermo_hi_enable(0);
      } else {
        val = atoi(shell_buf + arg2);
        thermo_hi_threshold_set(val);
      }
    } else if(!strcmp(shell_buf + arg1, "lo")) {
      if(!strcmp(shell_buf + arg2, "inc")) {
        thermo_lo_threshold_inc();
      } else if(!strcmp(shell_buf + arg2, "dec")) {
        thermo_lo_threshold_dec();
      } else if(!strcmp(shell_buf + arg2, "rst")) {
        thermo_lo_threshold_reset();
      } else if(!strcmp(shell_buf + arg2, "on")) {
        thermo_lo_enable(1);
      } else if(!strcmp(shell_buf + arg2, "off")) {
        thermo_lo_enable(0);
      } else {
        val = atoi(shell_buf + arg2);
        thermo_lo_threshold_set(val);
      }
    } else {
      return 1;
    }
  }
  return 0;
}
