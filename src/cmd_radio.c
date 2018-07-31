#include <string.h>
#include <stdlib.h>
#include "cmd_radio.h"
#include "shell.h"
#include "debug.h"
#include "radio.h"

#ifdef __CLOCK_DEBUG__

static void cmd_radio_cb(unsigned int freq)
{
  CDBG("freq %u, stereo %s\n", freq, radio_get_stereo() ? "ON" : "OFF");
}

char cmd_radio(char arg1, char arg2)
{
  unsigned int val;
  if(arg1 == 0 && arg2 == 0) {
    radio_dump();
    return 0;
  } else if(!strcmp(shell_buf + arg1, "on")) {
    radio_enable(1);
    return 0;
  } else if(!strcmp(shell_buf + arg1, "off")) {
    radio_enable(0);
    return 0;
  } else if(!strcmp(shell_buf + arg1, "station")) {
    if(arg2 == 0) {
      CDBG("freq: %u\n", radio_get_frequency());
    } else if(!strcmp(shell_buf + arg2, "prev")) {
      radio_prev_station(cmd_radio_cb);
    } else if(!strcmp(shell_buf + arg2, "next")) {
      radio_next_station(cmd_radio_cb);
    } else if(!strcmp(shell_buf + arg2, "dec")) {
      radio_dec_frequency();
    } else if(!strcmp(shell_buf + arg2, "inc")) {
      radio_inc_frequency();
    } else {
      val = atoi(shell_buf + arg2);
      if(val != 0) {
        radio_set_frequency(val);
        return 0;
      }
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "vol")) {
    if(arg2 == 0) {
      CDBG("vol: %bu\n", radio_get_volume());
    } else {
      val = atoi(shell_buf + arg2);
      radio_set_volume(val);
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "hlsi")) {
    if(arg2 == 0) {
      CDBG("hisi: %s\n", radio_get_hlsi() ? "HI" : "LO");
    } else if(!strcmp(shell_buf + arg2, "hi")){
      radio_set_hlsi(1);
    } else if(!strcmp(shell_buf + arg2, "lo")){
      radio_set_hlsi(0);
    } else {
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "ms")) {
    if(arg2 == 0) {
      CDBG("ms: %s\n", radio_get_ms() ? "ON" : "OFF");
    } else if(!strcmp(shell_buf + arg2, "on")){
      radio_set_ms(1);
    } else if(!strcmp(shell_buf + arg2, "off")){
      radio_set_ms(0);
    } else {
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "bl")) {
    if(arg2 == 0) {
      CDBG("bl: %s\n", radio_get_bl() == RADIO_BL_JAPNESE ? "JA" : "EU");
    } else if(!strcmp(shell_buf + arg2, "ja")){
      radio_set_bl(RADIO_BL_JAPNESE);
    } else if(!strcmp(shell_buf + arg2, "eu")){
      radio_set_bl(RADIO_BL_EUROPE);
    } else {
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "hcc")) {
    if(arg2 == 0) {
      CDBG("hcc: %s\n", radio_get_hcc() ? "ON" : "OFF");
    } else if(!strcmp(shell_buf + arg2, "on")){
      radio_set_hcc(1);
    } else if(!strcmp(shell_buf + arg2, "off")){
      radio_set_hcc(0);
    } else {
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "snc")) {
    if(arg2 == 0) {
      CDBG("snc: %s\n", radio_get_snc() ? "ON" : "OFF");
    } else if(!strcmp(shell_buf + arg2, "on")){
      radio_set_snc(1);
    } else if(!strcmp(shell_buf + arg2, "off")){
      radio_set_snc(0);
    } else {
      return 1;
    }
    return 0;
  } else if(!strcmp(shell_buf + arg1, "dtc")) {
    if(arg2 == 0) {
      CDBG("dtc: %s\n", radio_get_dtc() == RADIO_DTC_75US ? "75us" : "50us");
    } else if(!strcmp(shell_buf + arg2, "75")){
      radio_set_dtc(RADIO_DTC_75US);
    } else if(!strcmp(shell_buf + arg2, "50")){
      radio_set_dtc(RADIO_DTC_50US);
    } else {
      return 1;
    }
    return 0;
  }
  return 1;
}

#endif