#include <stdlib.h>
#include "cmd_serial.h"
#include "shell.h"
#include "debug.h"
#include "serial_hub.h"


char cmd_serial(char arg1, char arg2)
{
  char index, val;

  if(arg1 == 0) {
    serial_dump();
  } else {
    index = atoi(shell_buf + arg1);
    if(arg2 == 0) {
      printf("[%02bd] %c\n", index, serial_test_ctl_bit(index) ? '1' : '0');
    } else {
      val = atoi(shell_buf + arg2) != 0 ? 1 : 0;
      printf("[%02bd] %c -> %c\n", index, serial_test_ctl_bit(index) ? '1' : '0', val ? '1' : '0');
      serial_set_ctl_bit(index, val);
      serial_ctl_out();
    }
  }
  return 0;
}
