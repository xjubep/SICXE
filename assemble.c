#include "assemble.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcode.h"
#include "symbol.h"
#include <math.h>

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
    // 성공하면 1 리턴, 실패하면 -1 리턴
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
        return -1;
    }

    if (pass2(mid_filename, lst_filename, obj_filename) != 0) {
        return -1;
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
            if (tmp.operand[strlen(tmp.operand)-1] == ',')
                strcpy(tmp.operand2, tmp.token[3]);
            else
                strcpy(tmp.operand2, "");

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
            if (tmp.operand[strlen(tmp.operand)-1] == ',')
                strcpy(tmp.operand2, tmp.token[2]);
            else
                strcpy(tmp.operand2, "");
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

int is_register(char *operand) {
    if (strcmp(operand, "A") == 0)
        return 0;
    else if (strcmp(operand, "X") == 0)
        return 1;
    else if (strcmp(operand, "L") == 0)
        return 2;
    else if (strcmp(operand, "B") == 0)
        return 3;
    else if (strcmp(operand, "S") == 0)
        return 4;
    else if (strcmp(operand, "T") == 0)
        return 5;
    else if (strcmp(operand, "F") == 0)
        return 6;
    else if (strcmp(operand, "PC") == 0)
        return 8;
    else if (strcmp(operand, "SW") == 0)
        return 9;
    else
        return -1;
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
    int base = 0;
    int text_start_addr;

    if (!mid_fp || !lst_fp || !obj_fp) {
        /* file open error 처리, main 프로그램 종료 */
        fprintf(stderr, "file open error!\n");
        return 0;
    }

    if (strcmp(tmp[0].token[1], "START") == 0) {
        fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[0].LOCCTR, tmp[0].token[0], tmp[0].token[1], tmp[0].token[2]);
    }

    /* write header record */
    fprintf(obj_fp, "H%-6s%06X%06X\n", tmp[0].token[0], tmp[0].LOCCTR, program_len);
    text_start_addr = tmp[0].LOCCTR;

    int i = 0;
    char ret[10];
    char text_record[70];
    strcpy(text_record, "");

    while (1) {
        i++;
        line_idx += 5;

        strcpy(ret, "");

        if (strcmp(tmp[i].opcode, "END") == 0) {
            fprintf(lst_fp, "%4d %4s %-10s %-10s %-10s\n", line_idx, "", tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            break;
        }

        /* 주석이 아니면 */
        if (tmp[i].is_comment == 0) {
            /* optable에 존재하는 opcode이면 */
            if (op_find(tmp[i].opcode) != NULL) {
                op_node_ptr cur = op_find(tmp[i].opcode);

                /* operand가 2개일 경우 */
                if (tmp[i].operand[strlen(tmp[i].operand)-1] == ',') {
                    tmp[i].operand[strlen(tmp[i].operand)-1] = '\0';
                }

                /* operand에 symbol이 존재할 경우 */
                int sym = sym_find(tmp[i].operand);
                if (sym != -1 || tmp[i].operand[0] == '#' || tmp[i].operand[0] == '@'
                    || is_register(tmp[i].operand) != -1 || strcmp(tmp[i].opcode, "RSUB") == 0) {
                    if (tmp[i].format == 1) {
                        sprintf(ret, "%02X", cur->value);
                    }
                    else if (tmp[i].format == 2) {
                        int reg1, reg2;

                        /* register의 개수가 0개 */
                        if (strcmp(tmp[i].operand, "") == 0) {
                            reg1 = 0;
                            reg2 = 0;
                            sprintf(ret, "%02X%01X%01X", cur->value, reg1, reg2);
                        }
                        /* register의 개수가 1개 */
                        else if (strcmp(tmp[i].operand2, "") == 0) {
                            reg1 = is_register(tmp[i].operand);
                            reg2 = 0;
                            sprintf(ret, "%02X%01X%01X", cur->value, reg1, reg2);
                        }
                        /* register의 개수가 2개 */
                        else {
                            reg1 = is_register(tmp[i].operand);
                            reg2 = is_register(tmp[i].operand2);
                            sprintf(ret, "%02X%01X%01X", cur->value, reg1, reg2);
                        }
                    }
                    else if (tmp[i].format == 3) {
                        int N = 0, I = 0, X = 0, B = 0, P = 0, E = 0;
                        int PC;

                        /* default를 PC relative로 가정 */
                        P = 1;
                        PC = tmp[i+1].LOCCTR;
                        int disp;
                        // x
                        if (strcmp(tmp[i].operand2, "") != 0) {
                            X = 1;
                        }
                        
                        // indirect addressing
                        if (tmp[i].operand[0] == '@') {
                            N = 1;
                            disp = sym_find(tmp[i].operand+1) - PC;
                        }
                        // immediate addressing
                        else if (tmp[i].operand[0] == '#') {
                            I = 1;
                            int sym2 = sym_find(tmp[i].operand+1);
                            /* # 제거한게 심볼인 경우 */
                            if (sym2 != -1) {
                                disp = sym2 - PC;
                            }
                            /* # 제거한게 숫자인 경우 */
                            else {
                                P = 0;
                                disp = strtol(tmp[i].operand+1, NULL, 10);
                            }
                        }
                        // simple addressing
                        else {
                            N = 1;
                            I = 1;
                            disp = sym - PC;
                        }

                        // base relatvie
                        if (!(-2048 <= disp && disp <= 2047)) {
                            B = 1;
                            P = 0;
                            disp = sym- base;
                        }

                        /* 음수 처리 */
                        if (P == 1 && disp < 0) {
                            disp *= -1;
                            disp = 0x1000 - disp;
                        }

                        /* operand가 없을 경우 */
                        if (strcmp(tmp[i].operand, "") == 0) {
                            P = 0;
                            B = 0;
                            disp = 0;
                        }

                        sprintf(ret, "%02X%01X%03X", cur->value+N*2+I, X*8+B*4+P*2+E, disp);

                    }
                    else if (tmp[i].format == 4) {
                        int N = 0, I = 0, X = 0, B = 0, P = 0, E = 0;

                        int addr = sym_find(tmp[i].operand);
                        E = 1;
                        // x
                        if (strcmp(tmp[i].operand2, "") != 0) {
                            X = 1;
                        }
                        
                        // indirect addressing
                        if (tmp[i].operand[0] == '@') {
                            N = 1;
                        }
                        // immediate addressing
                        else if (tmp[i].operand[0] == '#') {
                            I = 1;
                            int sym2 = sym_find(tmp[i].operand+1);
                            /* # 제거한게 심볼인 경우 */
                            if (sym2 != -1) { // 이런 경우가 있나?
                                addr = sym2;
                            }
                            /* # 제거한게 숫자인 경우 */
                            else {
                                addr = strtol(tmp[i].operand+1, NULL, 10);
                            }
                        }
                        // simple addressing
                        else {
                            N = 1;
                            I = 1;
                        }

                        /* operand가 없을 경우 */
                        if (strcmp(tmp[i].operand, "") == 0) {
                            addr = 0;
                        }

                        /* format 4 출력 처리 */
                        if (tmp[i].format == 4) {
                            char buf[MX_TOKEN_LEN+1];

                            sprintf(buf, "+%s", tmp[i].opcode);
                            strcpy(tmp[i].opcode, buf);
                        }

                        sprintf(ret, "%02X%01X%05X", cur->value+N*2+I, X*8+B*4+P*2+E, addr);
                    }
                    /* operand가 1개인 경우 출력 */
                    if (strcmp(tmp[i].operand2, "") == 0) {
                        fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s %-10s\n", 
                        line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand, ret);
                    }
                    /* operand가 2개인 경우 출력 */
                    else {
                        strcat(tmp[i].operand, ", ");
                        strcat(tmp[i].operand, tmp[i].operand2);
                        fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s %-10s\n", 
                        line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand, ret);
                    }
                }
                else {
                    /* symbol이 존재하지 않는 경우 (undefined symbol) */
                    fprintf(stderr, "line %4d: undefined symbol\n", line_idx);
                    //return -1;
                }
            }
            else if (strcmp(tmp[i].opcode, "BYTE") == 0) {
                if (tmp[i].operand[0] == 'C') {
                    strcpy(ret, "");
                    int len = strlen(tmp[i].operand);
                    for (int j = 2; j < len-1; j++) {
                        char buf[10];
                        //printf("%02X", (int)tmp[i].operand[j]);
                        sprintf(buf, "%02X", (int)tmp[i].operand[j]);
                        strcat(ret, buf);
                    }
                }
                else if (tmp[i].operand[0] == 'X') {
                    int len = strlen(tmp[i].operand);
                    strncpy(ret, tmp[i].operand+2, len-3);
                    ret[len-3] = '\0';
                }

                fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand, ret);
            }
            else if (strcmp(tmp[i].opcode, "WORD") == 0) {
                fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            }
            else if (strcmp(tmp[i].opcode, "RESB") == 0) {
                fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            }
            else if (strcmp(tmp[i].opcode, "RESW") == 0) {
                fprintf(lst_fp, "%4d %04X %-10s %-10s %-10s\n", line_idx, tmp[i].LOCCTR, tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            }
            else if (strcmp(tmp[i].opcode, "BASE") == 0) {
                base = sym_find(tmp[i].operand);
                fprintf(lst_fp, "%4d %4s %-10s %-10s %-10s\n", line_idx, "", tmp[i].symbol, tmp[i].opcode, tmp[i].operand);
            }
            
            /* T 레코드에 처리 부분*/
            int text_len = strlen(text_record);
            
            if (text_len + strlen(ret) > 60 || (op_find(tmp[i].opcode) == NULL && op_find(tmp[i-1].opcode) == NULL)) {
                fprintf(obj_fp, "T%06X%02X%s\n", text_start_addr, text_len/2, text_record);
                strcpy(text_record, "");
                text_start_addr = tmp[i].LOCCTR;
            }
            strcat(text_record, ret);


        }
        else {
            /* 주석인 경우 */
            fprintf(lst_fp, "%4d %s\n", line_idx, tmp[i].statement);
        }
    }

    /* End 레코드 처리 부분 */

    fclose(mid_fp);
    fclose(lst_fp);   
    fclose(obj_fp);   

    return 0;
}