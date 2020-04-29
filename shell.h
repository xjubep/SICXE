#ifndef __SHELL_H
#define __SHELL_H

/* 명령어 한 개(ex. 'dump 14, 37'에서의 'dump', '14', '37')의 최대 길이 */
#define MX_CMD_LEN 21

void help(void);
void dir(void);
void quit(void);
int type(char *filename);

#endif
