#ifndef __MEMORY_H
#define __MEMORY_H

unsigned char mem[65536 * 16];

int dump(int cmd_num, char* start, char* end);
int edit(char* addr, char* val);
int fill(char* start, char* end, char* val);
void reset(void);

#endif