#include "run.h"
#include <stdio.h>
#include <string.h>

int bp(int cmd_num, char* addr) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    if (cmd_num > 2)
        return -1;

    if  (cmd_num == 1) {
        // print all bp
        bp_print();
    }
    else if (strcmp(addr, "clear") == 0) {
        // ex. bp clear
    }
    else {
        // ex. bp 3
    }

    return 1;
}

void bp_print(void) {
    int i;

    printf("%10s %10s\n", "", "breakpoint");
    printf("%10s ", "");
    for (i = 0; i < 10; i++)
        printf("-");
    printf("\n");

}
