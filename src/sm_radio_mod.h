#ifndef __CLOCK_SM_RADIO_MOD_H__
#define __CLOCK_SM_RADIO_MOD_H__

#include "sm.h"

enum sm_radio_mod_ss {
  SM_RADIO_MOD_INIT       = 0, //初始状态
  SM_RADIO_MOD_HLSI       = 1, //HIGH/LOW Side Injection: ON/OFF
  SM_RADIO_MOD_MS         = 2, //Mono to Stereo: ON/OFF
  SM_RADIO_MOD_BL         = 3, //Band Limits: Japanese FM band / US/Europe FM band
  SM_RADIO_MOD_HCC        = 4, //High Cut Control: ON/OFF
  SM_RADIO_MOD_SNC        = 5, //Stereo Noise Cancelling: ON/OFF
  SM_RADIO_MOD_DTC        = 6, // de-emphasis time constant is 75 μs or 50 us: 75/50
};

extern const char * code sm_radio_mod_ss_name[];

void sm_radio_mod_init(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod0(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod1(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod2(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod3(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod4(unsigned char from, unsigned char to, enum task_events);
void sm_radio_mod_submod5(unsigned char from, unsigned char to, enum task_events);
#endif