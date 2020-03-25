#ifndef __SHELL_H
#define __SHELL_H

#define MX_CMD_LEN 21

void help(void);
void dir(void);
void quit(void);
void history(int cmd_idx, char cmd_line[4*MX_CMD_LEN]);

void dump(int cmd_num, char** cmd);

#endif