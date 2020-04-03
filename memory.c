#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int dump0_idx = 0x00000;

int dump(int cmd_num, char* s, char* e) {
    unsigned int start, end, tmp_start, tmp_end;

    if (cmd_num == 1) {
        start = dump0_idx;
        end = start + 159;
        dump0_idx = end + 1;
    }
    else if (cmd_num == 2) {
        start = strtoul(s, NULL, 16);
        end = start + 159;
    }
    else if (cmd_num == 3) {
        start = strtoul(s, NULL, 16);
        end = strtoul(e, NULL, 16);
    }

    if (start > end) {
        return 0; // error 0(false)
    }

    tmp_start = start / 16 * 16;
    tmp_end = (end + 16) / 16 * 16;

    if (tmp_end > 0xFFFFF)
        tmp_end = 0xFFFFF;

    for (int addr = tmp_start; addr < tmp_end; addr += 16) {
        printf("%05X ", addr);

        for (int i = 0; i < 16; i++) {
            if (addr+i < start || addr+i > end)
                printf("   ");
            else
                printf("%02X ", mem[addr + i]);
        }
        printf("; ");
        for (int i = 0; i < 16; i++) {
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
    return 1;
}

int edit(char* a, char* v) {
    unsigned int addr = strtoul(a, NULL, 16);
    unsigned int val = strtoul(v, NULL, 16);

    if (addr < 0x00000 || addr > 0xFFFFF) {
        return 0; // error 0(false)
    }
    mem[addr] = (char)val;
    return 1;
}

int fill(char* s, char *e, char *v) {
    unsigned int start = strtoul(s, NULL, 16);
    unsigned int end = strtoul(e, NULL, 16);
    unsigned int val = strtoul(v, NULL, 16);

    for (unsigned int addr = start; addr <= end; addr++) {
        if (addr < 0x00000 || addr > 0xFFFFF) {
            return 0; // error 0(false)
        }
        mem[addr] = (char)val;
    }
    return 1;
}

void reset(void) {
    memset(mem, 0, sizeof(mem));
}