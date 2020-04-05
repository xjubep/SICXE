#ifndef __MEMORY_H
#define __MEMORY_H

/* 1MB의 가상 메모리 공간 */
unsigned char mem[65536 * 16];

int dump(int cmd_num, char* start, char* end);  // 입력 인자의 개수에 따라 메모리 값을 출력하는 함수
int edit(char* addr, char* val);                // 메모리 addr 번지의 값을 val 값으로 edit하는 함수
int fill(char* start, char* end, char* val);    // 메모리 start 번지부터 end 번지까지의 값을 val 값으로 fill하는 함수
void reset(void);                               // 메모리 전체를 전부 0으로 변경하는 함수

#endif