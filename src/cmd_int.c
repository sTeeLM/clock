#include <string.h>
#include "shell.h"
#include "cmd_int.h"
#include "int_hub.h"
#include "cext.h"

char cmd_int(char arg1, char arg2)
{
  unsigned int status;
  
  UNUSED_PARAM(arg2);
  
  if(arg1 == 0) {
    int_hub_dump();
  } else if(strcmp("ext", shell_buf + arg1) == 0){
    status = int_hub_get_status();
    int_hub_dump_ext_status(status);
  } else {
    return 1;
  }
  
  return 0;
}