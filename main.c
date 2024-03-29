#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include "list.h"
#include "opcode.h"
#include "memory.h"
#include "assemble.h"
#include "symbol.h"
#include "load.h"
#include "run.h"

unsigned int PROGADDR = 0x0000;       // sicsim 시작 후 progaddr의 default값

int main(void) {
    char cmd_line[4*MX_CMD_LEN];    // 명령어 한 줄 전체를 저장하는 변수
    char cmd[4][MX_CMD_LEN];        // 명령어를 파싱한 후 각각 저장하는 변수
    int cmd_num;                    // 파싱된 명령어의 개수 (ex. dump 14, 37 -> cmd_num=3)
    int i;

    /* history 관련 변수들 초기화 */
    List *hi = (List *)malloc(sizeof(List));    // history를 관리하는 linked list
    hi->size = 0;                               // history linked list의 size 초기화
    hi->front = NULL;                           // history linked list의 front 초기화
    hi->back = NULL;                            // history linked list의 back 초기화

    /* opcode table 관련 */
    FILE *fp = fopen("opcode.txt", "r");        // 'opcode.txt'를 가리키는 파일 포인터
    int tmp_value;                              // opcode value 저장 ('18 ADD 3/4' 에서 '18)
    char tmp_name[7], tmp_format[4];            // instruction name, format 저장 ('ADD', '3/4')

    /* bp 관련 변수들 초기화 */
    BP_List *bl = (BP_List *)malloc(sizeof(BP_List));   // bp를 관리하는 linked list;
    bl->size = 0;                                       // bp linked list의 size 초기화
    bl->front = NULL;                                   // bp linked list의 front 초기화
    bl->back = NULL;                                    // bp linked list의 back 초기화

    /* register */
    REG reg;

    if (!fp) {
        /* file open error 처리, main 프로그램 종료 */
        fprintf(stderr, "opcode file open error!\n");
        return 1;
    }

    while (fscanf(fp, "%X %s %s", &tmp_value, tmp_name, tmp_format) != EOF) {
        /*  opcode.txt를 line 단위로 읽어들이고 
            format에 따라 linked list 기반의 hash table에 저장 
            ('1':0, '2':1, '3/4':2) */
        if (strcmp(tmp_format, "1") == 0)
            op_insert(tmp_value, tmp_name, 0);
        else if (strcmp(tmp_format, "2") == 0)
            op_insert(tmp_value, tmp_name, 1);
        else if (strcmp(tmp_format, "3/4") == 0)
            op_insert(tmp_value, tmp_name, 2);
        else {
            /* undefined format의 경우 error 처리, main 프로그램 종료 */
            fprintf(stderr, "opcode format error!\n");
            return 1;
        }
    }

    /* opcode.txt 파일 닫기 */
    fclose(fp);

    while (1) {
        /* command line을 무한 반복해서 입력 받음 */
        printf("sicsim> ");
        fgets(cmd_line, sizeof(cmd_line), stdin);
        cmd_num = sscanf(cmd_line, "%s %s %s %s", cmd[0], cmd[1], cmd[2], cmd[3]);

        /* 입력 받은 command를 history linked list에 저장 */        
        push_back(hi, cmd_line);

        if (strcmp(cmd[0], "h") == 0 || strcmp(cmd[0], "help") == 0) {
            /* help command를 입력받으면 help 함수 호출 */
            help();
        }
        else if (strcmp(cmd[0], "d") == 0 || strcmp(cmd[0], "dir") == 0) {
            /* dir command를 입력받으면 dir 함수 호출 */
            dir();
        }
        else if (strcmp(cmd[0], "q") == 0 || strcmp(cmd[0], "quit") == 0) {
            /*  quit command를 입력받으면 quit 함수 호출 
                history linked list 메모리 해제 */            
            clear(hi);
            quit();
        }
        else if (strcmp(cmd[0], "hi") == 0 || strcmp(cmd[0], "history") == 0) {
            /* history command를 입력받으면 history linked list를 순회 */
            node_ptr his_cur = hi->front;
            for (i = 0; i < hi->size; i++) {
                printf("%5d %s", i+1, his_cur->data);
                his_cur = his_cur->next;
            }
        }
        else if (strcmp(cmd[0], "du") == 0 || strcmp(cmd[0], "dump") == 0) {
            if (dump(cmd_num, cmd[1], cmd[2]) == 0) {
                /* 잘못된 command가 입력된 경우이므로 history linked list에서 삭제 */
                printf("dump error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "e") == 0 || strcmp(cmd[0], "edit") == 0) {
            if (edit(cmd[1], cmd[2]) == 0) {
                /* 잘못된 command가 입력된 경우이므로 history linked list에서 삭제 */
                /* 주소값이 메모리 범위를 벗어나거나, value값이 범위를 벗어난 경우 */
                printf("edit error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "f") == 0 || strcmp(cmd[0], "fill") == 0) {
            if (fill(cmd[1], cmd[2], cmd[3]) == 0) {
                /* 잘못된 command가 입력된 경우이므로 history linked list에서 삭제 */
                /* 주소값이 메모리 범위를 벗어나거나, value값이 범위를 벗어난 경우 */
                printf("fill error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "reset") == 0) {
            /* reset command를 입력받으면 reset 함수 호출 */
            reset();
        }
        else if (strcmp(cmd[0], "opcode") == 0) {
            op_node_ptr tmp = op_find(cmd[1]);
            if (cmd_num != 2 || tmp == NULL) {
                /* 잘못된 command가 입력된 경우이므로 history linked list에서 삭제 */
                printf("opcode find error!\n");
                pop_back(hi);
            }
            else {
                printf("opcode is %02X\n", tmp->value);
            }
        }
        else if (strcmp(cmd[0], "opcodelist") == 0) {
            /* opcodelist command를 입력받으면 op_print_all 함수 호출 */
            op_print_all();
        }
        else if (strcmp(cmd[0], "type") == 0) {
            /* type command를 입력받으면 type 함수 호출 */
            if (cmd_num != 2 || type(cmd[1]) == 0) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("type error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "assemble") == 0) {
            /* assemble command를 입력받으면 assemble 함수 호출 */
            if (cmd_num != 2 || assemble(cmd[1]) == -1) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("assemble error!\n");
                pop_back(hi);

                /* symbol table clear */
                sym_clear();
            }          
        }
        else if (strcmp(cmd[0], "symbol") == 0) {
            /* symbol command를 입력받으면 symbol 함수 호출 */
            if (cmd_num != 1 || sym_print_all() == 0) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("symbol error!\n");
                pop_back(hi);
            }          
        }
        else if (strcmp(cmd[0], "progaddr") == 0) {
            /* progaddr command를 입력받은 경우 */
            if (cmd_num != 2 || strtoul(cmd[1], NULL, 16) > 0xFFFF) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("progaddr error!\n");
                pop_back(hi);
            }
            else {
                PROGADDR = strtoul(cmd[1], NULL, 16);
            }
        }
        else if (strcmp(cmd[0], "loader") == 0) {
            /* loader command를 입력받으면 loader 함수 호출 */
            if (loader(cmd_num, cmd[1], cmd[2], cmd[3], PROGADDR) == -1) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("loader error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "bp") == 0) {
            /* bp command를 입력받으면 bp 함수 호출 */
            if (bp(cmd_num, cmd[1], bl) == -1) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("bp error!\n");
                pop_back(hi);
            }
        }
        else if (strcmp(cmd[0], "run") == 0) {
            /* run command를 입력받으면 run 함수 호출 */
            if (cmd_num != 1 || run(bl, &reg, PROGADDR) == -1) {
                /* 잘못된 command가 입력 된 경우이므로 history linked list에서 삭제 */
                printf("run error!\n");
                pop_back(hi);
            }
        }
        else {
            /* 정의되지 않은 command가 입력된 경우이므로 history linked list에서 삭제 */
            pop_back(hi);
        }
    }

    return 0;
}
