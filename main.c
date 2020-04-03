#include <stdio.h>
#include <string.h> // strcmp
#include <stdlib.h>
#include "shell.h" // exit
#include "list.h"
#include "opcode.h"
#include "memory.h"

int main(void) {
    char cmd_line[4*MX_CMD_LEN];
    char cmd[4][MX_CMD_LEN];
    int cmd_num;

    //// history
    int cmd_idx = 0;
    List *hi = (List *)malloc(sizeof(List));
    hi->size = 0;
    hi->front = NULL;
    hi->back = NULL;
    ////

    //// opcode
    FILE *fp = fopen("opcode.txt", "r");
    int tmp_value;
    char tmp_name[7], tmp_format[4];

    if (!fp) {
        fprintf(stderr, "opcode file open error!\n");
        return 1;
    }

    while (fscanf(fp, "%X %s %s", &tmp_value, tmp_name, tmp_format) != EOF) {
        if (strcmp(tmp_format, "1") == 0)
            op_insert(tmp_value, tmp_name, 0);
        else if (strcmp(tmp_format, "2") == 0)
            op_insert(tmp_value, tmp_name, 1);
        else if (strcmp(tmp_format, "3/4") == 0)
            op_insert(tmp_value, tmp_name, 2);
        else {
            fprintf(stderr, "opcode format error!\n");
            return 1;
        }
    }
    fclose(fp);
    ////

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
            node_ptr his_cur = hi->front;
            for (int i = 0; i < cmd_idx; i++) {
                printf("%5d %s", i+1, his_cur->data);
                his_cur = his_cur->next;
            }
        }
        else if (strcmp(cmd[0], "du") == 0 || strcmp(cmd[0], "dump") == 0) {
            if (dump(cmd_num, cmd[1], cmd[2]) == 0) {
                printf("dump error!\n");
                pop_back(hi);
                cmd_idx--;                
            }
        }
        else if (strcmp(cmd[0], "e") == 0 || strcmp(cmd[0], "edit") == 0) {
            if (edit(cmd[1], cmd[2]) == 0) {
                printf("edit error!\n");
                pop_back(hi);
                cmd_idx--;                
            }
        }
        else if (strcmp(cmd[0], "f") == 0 || strcmp(cmd[0], "fill") == 0) {
            if (fill(cmd[1], cmd[2], cmd[3]) == 0) {
                printf("fill error!\n");
                pop_back(hi);
                cmd_idx--;                
            }
        }
        else if (strcmp(cmd[0], "reset") == 0) {
            reset();
        }
        else if (strcmp(cmd[0], "opcode") == 0) {
            if(op_find(cmd[1]) == 0) { // opcode.txt에 존재하지 않을 경우
                printf("opcode find error!\n");
                // 잘못된 명령어가 입력된 경우임
                pop_back(hi);
                cmd_idx--; 
            }
        }
        else if (strcmp(cmd[0], "opcodelist") == 0) {
            op_print_all();
        }
        else {
            // 잘못된 명령어가 입력된 경우, 히스토리 리스트에서 삭제
            pop_back(hi);
            cmd_idx--;
        }
    }

    return 0;
}