#include "assemble.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcode.h"
#include "symbol.h"

int LOCCTR = 0;
int program_len = -1;
state_info tmp[500];

/*  filename에 해당하는 소스 파일(*.asm)을 읽어서 
    object 파일(*.obj)과 리스팅 파일(*.lst)을 만든다. 
    소스파일에 에러가 존재할 경우, *.lst, *.obj 생성 않고
    에러 화면에 출력함. 에러 발생시 바로 명령 종료
    에러 내용은 디버깅을 위해 어떤 라인에서 에러 발생했는지 출력
    라인 번호는 5의 배수 단위로 지정하여 출력, 각 문자열의 길이는 30자 이내로 가정
    label은 영문, 숫자를 input, 그외 특수문자는 고려x
    어셈블 에러는
    1. 정의되지 않은 변수명이 사용되었을 경우  
    2. 변수명이 중복으로 사용되었을 경우
    3. 정의되지 않은 instruction이 사용되었을 경우 
*/
int assemble(char *filename) {
    // 성공하면 1 리턴, 실패하면 0 리턴
    char asm_filename[MX_FILENAME_LEN+1], obj_filename[MX_FILENAME_LEN+1], 
        lst_filename[MX_FILENAME_LEN+1], mid_filename[MX_FILENAME_LEN+1];
    filename = strtok(filename, ".");
    
    /* .asm, .obj, .lst, .mid 파일 이름 생성 */
    strcpy(asm_filename, filename);
    strcat(asm_filename, ".asm");
    strcpy(obj_filename, filename);
    strcat(obj_filename, ".obj");
    strcpy(lst_filename, filename);
    strcat(lst_filename, ".lst");
    strcpy(mid_filename, filename);
    strcat(mid_filename, ".mid");

    /* pass1 수행, 성공: program len, 실패:-1 리턴 */
    program_len = pass1(asm_filename, mid_filename);
    if (program_len == -1) {
        return 0;
    }

    if (pass2(mid_filename, lst_filename, obj_filename) == -1) {
        return 0;
    }

    return 1;
}

state_info get_statement_info(char *statement) {
    state_info tmp;
    tmp.token_num = sscanf(statement, "%s %s %s %s", tmp.token[0], tmp.token[1], tmp.token[2], tmp.token[3]);

    if (statement[0] == '.') {
        tmp.is_comment = 1;
        strcpy(tmp.statement, statement);
        tmp.statement[strlen(tmp.statement)-1] = '\0';
    }
    else {
        tmp.is_comment = 0;
        /* start행은 따로 처리할 거임 */

        /*  첫 번째 토큰이 opcode list에 존재하지 않으면서
            token의 개수가 3개인 경우 symbol이 있는 statement로 간주 */
        if (tmp.token_num == 3 && op_find(tmp.token[0]) == NULL) {
            // START일 경우에는 symbol에 집어 넣지 않음
            if (strcmp(tmp.token[1], "START") == 0)
                return tmp; 
            strcpy(tmp.symbol, tmp.token[0]);
            strcpy(tmp.opcode, tmp.token[1]);
            strcpy(tmp.operand, tmp.token[2]);
            strcpy(tmp.operand2, tmp.token[3]);

            if (sym_find(tmp.symbol) != -1) {
                /* SYMTAB에서 symbol을 찾으면 error flag */
                fprintf(stderr, "[ERROR] duplicate symbol: %s\n", tmp.symbol);
                //return NULL;
            }
            else {
                /* 처음 보는 symbol일 경우 SYMTAB에 추가 */
                sym_insert(tmp.symbol, LOCCTR);                
            }
        }
        else {
            strcpy(tmp.symbol, "");
            strcpy(tmp.opcode, tmp.token[0]);
            strcpy(tmp.operand, tmp.token[1]);
            strcpy(tmp.operand2, tmp.token[2]);
        }

        /* opcode format ('1':0, '2':1, '3/4':2) */
        if (tmp.opcode[0] == '+') {
            tmp.format = 4;
            strcpy(tmp.opcode, tmp.opcode+1);
        }
        else {
            op_node_ptr tmp_op = op_find(tmp.opcode);
            if (tmp_op != NULL) {
                tmp.format = tmp_op->format + 1;
            }
        }
    }

    return tmp;
}

int pass1(char *asm_filename, char *mid_filename) {
    // pass1 수행, 성공하면 program length 리턴, 실패하면 -1 리턴
    int program_len = -1;
    unsigned int start_addr = 0;
    char statement[MX_STATEMENT_LEN+1];
    int line_idx = 5;
    //state_info tmp[500];
    FILE *asm_fp = fopen(asm_filename, "r");
    FILE *mid_fp = fopen(mid_filename, "w");

    if (!asm_fp || !mid_fp) {
        /* file open error 처리, main 프로그램 종료 */
        fprintf(stderr, "file open error!\n");
        return 0;
    }
    // (symbol), opcode, operand, + loc, + object_code

    /* read first input line */
    fgets(statement, sizeof(statement), asm_fp);
    tmp[0] = get_statement_info(statement);
    /* 에러, 변수명 중복 사용*/
    //if (tmp == NULL) {
    //    return -1;
    //}

    if (strcmp(tmp[0].token[1], "START") == 0) {
        /*  if OPCODE = 'START', 
            save #[OPERAND] as starting addr, initialize LOCCTR to starting addr
            write line to mid file, read next input line
        */

        LOCCTR = strtol(tmp[0].token[2], NULL, 16);
        start_addr = LOCCTR;
        tmp[0].LOCCTR = LOCCTR;
        fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[0].LOCCTR, tmp[0].token[0], tmp[0].token[1], tmp[0].token[2]);
    }
    else {
        LOCCTR = 0;
        tmp[0].LOCCTR = LOCCTR;
    }

    int i = 0;
    while(1) {
        line_idx += 5;
        i++;

        fgets(statement, sizeof(statement), asm_fp);
        tmp[i] = get_statement_info(statement);
        /* 에러, 변수명 중복 사용*/
        //if (tmp == NULL) {
        //    return -1;
        //}
        tmp[i].LOCCTR = LOCCTR;

        if (strcmp(tmp[i].token[0], ".") != 0) {
            if (strcmp(tmp[i].opcode, "END") == 0) {
                fprintf(mid_fp, "%4d %4s %-10s %-10s %-10s\n", line_idx, "", tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
                break;
            }
        
            //printf("%4d %04X %-10s %-10s %-10s\n", line_idx, LOCCTR, tmp.symbol, tmp.opcode, tmp.operand);

            if (strcmp(tmp[i].opcode, "WORD") == 0) {
                fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
                LOCCTR += 3;
            }
            else if (strcmp(tmp[i].opcode, "RESW") == 0) {
                fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
                LOCCTR += 3 * strtol(tmp[i].operand, NULL, 10);
            }
            else if (strcmp(tmp[i].opcode, "RESB") == 0) {
                fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
                LOCCTR += strtol(tmp[i].operand, NULL, 10);
            }
            else if (strcmp(tmp[i].opcode, "BYTE") == 0) {
                fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);

                int len = strlen(tmp[i].operand);
                if (tmp[i].operand[0] == 'X') {
                    LOCCTR += (len-3)/2;
                }
                else if (tmp[i].operand[0] == 'C') {
                    LOCCTR += (len-3);
                }
            }
            else if (op_find(tmp[i].opcode) == NULL) {
                /* directive의 경우 object code로 변환 x, opcode앞에 '+'붙은 경우도 고려 */
                fprintf(mid_fp, "%4d %4s %-10s %-10s %-10s\n", line_idx, "", tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            }
            else {
                /* 일반적인 opcode의 경우 */
                if (tmp[i].operand[strlen(tmp[i].operand)-1] == ',') {
                    fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s %-3s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand, tmp[i].operand2);
                }
                else {
                    if (strcmp(tmp[i].opcode, "RSUB") == 0) {
                        strcpy(tmp[i].operand, "");
                    }

                    /* format 4 출력 처리 */
                    if (tmp[i].format == 4) {
                        char buf[MX_TOKEN_LEN+1];

                        sprintf(buf, "+%s", tmp[i].opcode);
                        strcpy(tmp[i].opcode, buf);
                    }
                    fprintf(mid_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
                    
                    if (tmp[i].format == 4) {
                        strcpy(tmp[i].opcode, tmp[i].opcode+1);
                    }
                }
                LOCCTR += tmp[i].format;
            }           
        }
        else {
            /* 주석인 경우 */
            fprintf(mid_fp, "%4d %4s %-s\n", line_idx, "", tmp[i].statement);
        }
    }

    program_len = LOCCTR - start_addr;

    fclose(asm_fp);
    fclose(mid_fp);   

    return program_len;
}

int pass2(char *mid_filename, char *lst_filename, char *obj_filename) {
    FILE *mid_fp = fopen(mid_filename, "r");
    FILE *lst_fp = fopen(lst_filename, "w");
    FILE *obj_fp = fopen(obj_filename, "w");

    int line_idx = 5;

    if (!mid_fp || !lst_fp || !obj_fp) {
        /* file open error 처리, main 프로그램 종료 */
        fprintf(stderr, "file open error!\n");
        return 0;
    }

    if (strcmp(tmp[0].opcode, "START") == 0) {
        fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[0].LOCCTR, tmp[0].token[0], tmp[0].token[1], tmp[0].token[2]);
    }
    /* write header record */
    fprintf(obj_fp, "H%-6s%06X%06X\n", tmp[0].token[0], tmp[0].LOCCTR, program_len);

    int i = 0;
    while (1) {
        i++;
        line_idx += 5;

        if (strcmp(tmp[i].opcode, "END") == 0) {
            fprintf(lst_fp, "%4d %4s %-10s %-10s %-10s\n", line_idx, "", tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            break;
        }

        /* 주석이 아니면 */
        if (tmp[i].is_comment == 0) {
            /* optable에 존재하는 opcode이면 */
            if (op_find(tmp[i].opcode) != NULL) {
                /* operand가 2개일 경우 */
                if (tmp[i].operand[strlen(tmp[i].operand)-1] == ',') {
                    tmp[i].operand[strlen(tmp[i].operand)-1] = '\0';
                    //printf("%s %s\n", tmp[i].operand, tmp[i].operand2);
                }
                //else {
                //    printf("%s\n", tmp[i].operand);
                //}

                /* operand에 symbol이 존재할 경우 */
                if (sym_find(tmp[i].operand) != -1) {

                }
            }
        }
    }


    fclose(mid_fp);
    fclose(lst_fp);   
    fclose(obj_fp);   

    return -1;
}