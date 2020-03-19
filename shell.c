#include "shell.h"
#include <stdio.h>
#include <stdlib.h>

void help(void) {
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}

void dir(void) {
    printf("dir\n");
}

void quit(void) {
    exit(0);
}

void history(void) {
    printf("history\n");
}

void dump(int cmd_num, char** cmd) {
    if (cmd_num == 1) {
        printf("dump\n");
    }
    else if (cmd_num == 2) {
        printf("dump start\n");
    }  
    else if (cmd_num == 3) {
        printf("dump start, end\n");
    }
}
