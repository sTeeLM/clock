#include <string.h>
#include <stdlib.h>
#include "cmd_power.h"
#include "shell.h"
#include "debug.h"
#include "power.h"

#ifdef __CLOCK_DEBUG__

char cmd_power(char arg1, char arg2)
{
  unsigned int flt;
  unsigned int val;
  if(arg1 != 0) {
    if(strcmp(shell_buf + arg1, "5v") == 0) {
      if(arg2 != 0) {
        if(strcmp(shell_buf + arg2, "on") == 0) {
          power_5v_enable(1);
        } else if (strcmp(shell_buf + arg2, "off") == 0) {
          power_5v_enable(0);
        } 
      } else {
        CDBG(("power 5v is %s\n", power_5v_get_enable()? "ON" : "OFF"));
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "hi") == 0) {
      if(arg2 == 0) {
        val = power_get_alert_vhigh();
        flt = power_hex2float(val);
        CDBG(("power alert hi is 0x%04x, %bu.%02bu V\n", 
            val, (unsigned char)(flt / 100), (unsigned char)(flt % 100) ));
      } else {
        val = atoi(shell_buf + arg2);
        flt = power_hex2float(val);
        CDBG(("power alert hi set to 0x%04x, %bu.%02bu V\n", 
          val, (unsigned char)(flt / 100), (unsigned char)(flt % 100) ));
        power_set_alert_vhigh(val);
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "lo") == 0) {
      if(arg2 == 0) {
        val = power_get_alert_vlow();
        flt = power_hex2float(val);
        CDBG(("power alert low is 0x%04x, %bu.%02bu V\n", 
            val, (unsigned char)(flt / 100), (unsigned char)(flt % 100)));
      } else {
        val = atoi(shell_buf + arg2);
        flt = power_hex2float(val);
        CDBG(("power alert low set to 0x%04x, %bu.%02bu V\n", 
          val, (unsigned char)(flt / 100), (unsigned char)(flt % 100)));
        power_set_alert_vlow(val);
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "hyst") == 0) {
      if(arg2 == 0) {
        val = power_get_hyst();
        flt = power_hex2float(val);
        CDBG(("power hyst is 0x%04x, %bu.%02bu V\n", 
            val, (unsigned char)(flt / 100), (unsigned char)(flt % 100)));
      } else {
        val = atoi(shell_buf + arg2);
        flt = power_hex2float(val);
        CDBG(("power hyst set to 0x%04x, %bu.%02bu V\n", 
          val, (unsigned char)(flt / 100), (unsigned char)(flt % 100)));
        power_set_hyst(val);
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "int") == 0) {
      if(arg2 == 0) {
        CDBG(("alert: hi = %s lo = %s\n", power_test_high_alert() ? "1" : "0", 
        power_test_low_alert() ? "1" : "0"));
      } else if(strcmp(shell_buf + arg2, "clr") == 0) {
        power_clr_high_alert();
        power_clr_low_alert();
      } else {
        return 1;
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "cal") == 0) {
      if(arg2 == 0) {
        CDBG(("power: calibration is %s\n", power_get_calibration() ? "ON" : "OFF"));
      } else if(strcmp(shell_buf + arg2, "on") == 0) {
        power_set_calibration(1);
      } else if(strcmp(shell_buf + arg2, "off") == 0) {
        power_set_calibration(0);
      } else {
        return 1;
      }
      return 0;
    } else if(strcmp(shell_buf + arg1, "bat") == 0 && arg2 == 0) {
      flt = power_get_voltage();
      CDBG(("battery is %bu%% %bu.%bu V\n", power_get_percent(), (unsigned char)(flt / 100), (unsigned char)(flt % 100)));
      return 0;
    } else {
      return 1;
    }
  }
  return 1;
}

#endif