#include <string.h>
#include <stdlib.h>
#include "cmd_mpu.h"
#include "shell.h"
#include "debug.h"
#include "mpu.h"
#include "com.h"


char cmd_mpu(char arg1, char arg2)
{
  if(arg1 != 0 && arg2 == 0 && strcmp("on", shell_buf + arg1) == 0) {
    mpu_enable(1);
  } else if(arg1 != 0 && arg2 == 0 && strcmp("off", shell_buf + arg1) == 0) {
    mpu_enable(0);  
  } else if(arg1 != 0 && arg2 == 0 && strcmp("int", shell_buf + arg1) == 0) {
    printf("interrupt status = %02bx\n", mpu_read_int_status());
  } else {
    return 1;
  }
  return 0;
}
