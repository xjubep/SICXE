#include <stdio.h>
#include <string.h> // strcmp
#include <stdlib.h>
#include "shell.h" // exit
#include "list.h"

int main(void) {
    char cmd_line[4*MX_CMD_LEN];
    char cmd[4][MX_CMD_LEN];
    int cmd_num, cmd_idx = 0;
    List *hi = (List *)malloc(sizeof(List));
    hi->size = 0;
    hi->front = NULL;
    hi->back = NULL;

    while (1) {
        printf("sicsim> ");
        fgets(cmd_line, sizeof(cmd_line), stdin);
        cmd_num = sscanf(cmd_line, "%s %s %s %s", cmd[0], cmd[1], cmd[2], cmd[3]);
        cmd_idx++;

        push_back(hi, cmd_line);

        if (strcmp(cmd[0], "h") == 0 || strcmp(cmd[0], "help") == 0) {
            help();
        }
        else if (strcmp(cmd[0], "d") == 0 || strcmp(cmd[0], "dir") == 0) {
            dir();
        }
        else if (strcmp(cmd[0], "q") == 0 || strcmp(cmd[0], "quit") == 0) {
            quit();
        }
        else if (strcmp(cmd[0], "hi") == 0 || strcmp(cmd[0], "history") == 0) {
            node_ptr cur = hi->front;
            for (int i = 0; i < cmd_idx; i++) {
                printf("%5d %s", i+1, cur->data);
                cur = cur->next;
            }
        }
        else if (strcmp(cmd[0], "du") == 0 || strcmp(cmd[0], "dump") == 0) {
            dump(cmd_num, cmd);
        }
        else if (strcmp(cmd[0], "e") == 0 || strcmp(cmd[0], "edit") == 0) {
            printf("edit\n");
            printf("address: %s\n", cmd[1]);
            printf("value: %s\n", cmd[2]);
        }
        else if (strcmp(cmd[0], "f") == 0 || strcmp(cmd[0], "fill") == 0) {
            printf("fill\n");
            printf("start: %s\n", cmd[1]);
            printf("end: %s\n", cmd[2]);
            printf("value: %s\n", cmd[3]);
        }
        else if (strcmp(cmd[0], "reset") == 0) {
            printf("reset\n");
        }
        else if (strcmp(cmd[0], "opcode") == 0) {
            printf("opcode %s\n", cmd[1]);
        }
        else if (strcmp(cmd[0], "opcodelist") == 0) {
            printf("opcodelist\n");
        }
        else {
            // 잘못된 명령어가 입력된 경우, 히스토리 리스트에서 삭제
            pop_back(hi);
            cmd_idx--;
        }
    }

    return 0;
}