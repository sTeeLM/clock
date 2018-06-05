#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "i2c.h"
#include "shell.h"
#include "cmd_i2c.h"

static unsigned char write_addr;

static char i2c_read(char arg1, char arg2)
{
  unsigned char addr, cmd, val;
  bit ret;
  
  if(arg1 == 0 || arg2 == 0)
    return 1;
  
  addr = atoi(shell_buf + arg1);
  cmd  = atoi(shell_buf + arg2);
  
  if(addr > 255 || cmd > 255)
    return 1;
  
  I2C_Init();
  ret = I2C_Get(addr, cmd, &val);
  
  printf("i2c read: [%02bx %02bx] -> %02bx %s\n", addr, cmd, val, ret ? "FAILED" : "SUCCESS");
  
  return 0;
}

static char i2c_write(char arg1, char arg2)
{
  unsigned char addr, cmd, val;
  bit ret;
  
  if(arg1 == 0)
    return 1;
  
  // iw addr : set write addr
  if(arg2 == 0) {
    addr = atoi(shell_buf + arg1);
    if(addr > 255) {
      return 1;
    } else {
      printf("i2c addr: [%02bx] -> [%02bx]\n", write_addr, addr);
      write_addr = addr;
      return 0;
    }
  }
  
  // iw cmd data: write data
  cmd = atoi(shell_buf + arg1);
  val = atoi(shell_buf + arg2);
  
  if(cmd > 255 || val > 255)
    return 1;
  
  I2C_Init();
  ret = I2C_Put(write_addr, cmd, val);
  printf("i2c write [%02bx %02bx] -> %02bx %s\n", write_addr, cmd, val, ret ? "FAILED" : "SUCCESS");
  
  return 0;
}

char cmd_i2c(char arg1, char arg2)
{
  if(strcmp(shell_buf, "ir") == 0) {
    return i2c_read(arg1, arg2);
  } else if(strcmp(shell_buf, "iw") == 0){
    return i2c_write(arg1, arg2);
  }
  return 1;
}
