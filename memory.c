#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int dump0_idx = 0x00000;   // parameter가 없는 dump 명령어에서 위치 저장을 위해 사용

int dump(int cmd_num, char* s, char* e) {
    unsigned int start, end, tmp_start, tmp_end, addr;
    int i;

    if (cmd_num == 1) {
        /* parameter의 개수가 0인 경우, dump 제외 */
        start = dump0_idx;
        end = start + 159;
        /* memory 범위를 벗어난 경우 다시 0번지부터 출력 */
        if (end > 0xFFFFF)
            dump0_idx = 0;
        else
            dump0_idx += 160;
    }
    else if (cmd_num == 2) {
        /* parameter의 개수가 1인 경우, dump 제외 */
        start = strtoul(s, NULL, 16);
        end = start + 159;
    }
    else if (cmd_num == 3) {
        /* parameter의 개수가 2인 경우, dump 제외 */
        start = strtoul(s, NULL, 16);
        end = strtoul(e, NULL, 16);
    }

    /* start 주소값이 메모리 범위를 벗어난 경우 error 처리 (false) */
    if (start > 0xFFFFF)
        return 0;    

    /* start 주소가 end 주소보다 큰 값이 들언온 경우 error 처리 (false)*/
    if (start > end) 
        return 0;     

    /* 가장 왼쪽 칼럼을 0 단위로 출력하기 위한 변수 */
    tmp_start = start / 16 * 16;
    tmp_end = (end + 16) / 16 * 16;

    /* 주소를 넘어난 경우 주소의 끝까지 출력 */
    if (tmp_end > 0xFFFFF)
        tmp_end = 0xFFFFF;

    for (addr = tmp_start; addr < tmp_end; addr += 16) {
        printf("%05X ", addr);

        for (i = 0; i < 16; i++) {
            if (addr+i < start || addr+i > end)
                printf("   ");
            else
                printf("%02X ", mem[addr + i]);
        }
        printf("; ");
        for (i = 0; i < 16; i++) {
            if (addr+i < start || addr+i > end)
                printf(".");
            else {
                if (mem[addr + i] < 0x20 || mem[addr + i] > 0x7E)
                    printf(".");
                else                
                    printf("%c", mem[addr + i]);
            }
        }
        printf("\n");
    }

    /* 정상적으로 dump 명령을 수행한 경우 1 (true) 리턴 */
    return 1;
}

/* 메모리 addr 번지의 값을 val 값으로 edit하는 함수 */
int edit(char* a, char* v) {
    unsigned int addr = strtoul(a, NULL, 16);
    unsigned int val = strtoul(v, NULL, 16);

    /* value 값이 범위를 벗어난 경우 error 처리 */
    if (val > 0xFF)
        return 0;

    /* address 값이 범위를 벗어난 경우 error 처리 */
    if (addr > 0xFFFFF)
        return 0;

    mem[addr] = (char)val;

    /* edit 성공 후 1(true) 리턴 */
    return 1;
}

/* 메모리 start 번지부터 end 번지까지의 값을 val 값으로 fill하는 함수 */
int fill(char* s, char *e, char *v) {
    unsigned int start = strtoul(s, NULL, 16);
    unsigned int end = strtoul(e, NULL, 16);
    unsigned int val = strtoul(v, NULL, 16);
    unsigned int addr;

    /* value 값이 범위를 벗어난 경우 error 처리 */
    if (val > 0xFF)
        return 0;

    /* start 주소값이 메모리 범위를 벗어난 경우 error 처리 (false) */
    if (start > 0xFFFFF)
        return 0;    

    /* start 주소가 end 주소보다 큰 값이 들언온 경우 error 처리 (false)*/
    if (start > end) 
        return 0;     

    for (addr = start; addr <= end; addr++) {
        /* address 값이 범위를 벗어난 경우 error 처리 */
        if (addr > 0xFFFFF) 
            return 0;         
        mem[addr] = (char)val;
    }
    /* fill 성공 후 1(true) 리턴 */
    return 1;
}

/* 메모리 전체를 전부 0으로 변경하는 함수 */
void reset(void) {
    memset(mem, 0, sizeof(mem));
}

/* 메모리 addr 번지의 값을 val 값으로 edit하는 함수 
    - parameter가 unsigned int */
int edit_uint(unsigned int addr, unsigned int val) {
    /* value 값이 범위를 벗어난 경우 error 처리 */
    if (val > 0xFF)
        return 0;

    /* address 값이 범위를 벗어난 경우 error 처리 */
    if (addr > 0xFFFFF)
        return 0;

    mem[addr] = (char)val;

    /* edit 성공 후 1(true) 리턴 */
    return 1;
}