#ifndef __CLOCK_SHELL_H__
#define __CLOCK_SHELL_H__

typedef char (code *SHELL_CMD_PROC)(char arg1, char arg2);

struct shell_cmds
{
  char * cmd;
  char * desc;
  char * usage;
  SHELL_CMD_PROC proc;
};

void run_shell(void);
char shell_search_cmd_by_name(char * cmd);
void shell_dump_cmd(void);
extern char shell_buf[];
extern struct shell_cmds code cmds[];
#endif
