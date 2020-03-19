#ifndef __SHELL_H
#define __SHELL_H

void help(void);
void dir(void);
void quit(void);
void history(void);

void dump(int cmd_num, char** cmd);

#endif