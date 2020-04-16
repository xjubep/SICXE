#include "assemble.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "opcode.h"
#include "symbol.h"

#define MX_FILENAME_LEN 30
#define MX_STATEMENT_LEN 240
#define MX_TOKEN_LEN 30

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

    /* pass1 수행 */
    if (pass1(asm_filename, mid_filename) == 0) {
        return 0;
    }

    return 1;
}

int pass1(char *asm_filename, char *mid_filename) {
    // pass1 수행, 성공하면 program length 리턴, 실패하면 -1 리턴
    int program_len = -1;
    unsigned int start_addr = 0;
    FILE *asm_fp = fopen(asm_filename, "r");
    FILE *mid_fp = fopen(mid_filename, "w");

    if (!asm_fp || !mid_fp) {
        /* file open error 처리, main 프로그램 종료 */
        fprintf(stderr, "file open error!\n");
        return 0;
    }

    //while (fscanf(asm_fp, "%s %s %s", &tmp_value, tmp_name, tmp_format) != EOF) {

    //}
    // (symbol), opcode, operand, + loc, + object_code

    char statement[MX_STATEMENT_LEN+1];
    int token_num;
    char token[7][MX_TOKEN_LEN+1];
    char symbol[MX_TOKEN_LEN+1], opcode[MX_TOKEN_LEN+1], operand[MX_TOKEN_LEN+1];
    int LOCCTR = 0, line_idx = 5, format = 3;

    /* read first input line */
    fgets(statement, sizeof(statement), asm_fp);
    token_num = sscanf(statement, "%s %s %s %s", token[0], token[1], token[2], token[3]);
    if (strcmp(token[1], "START") == 0) {
        /*  if OPCODE = 'START', 
            save #[OPERAND] as starting addr
            initialize LOCCTR to starting addr
            write line to mid file
            read next input line
        */
        strcpy(symbol, token[0]);
        strcpy(opcode, token[1]);
        strcpy(operand, token[2]);

        LOCCTR = strtoul(token[2], NULL, 16);
        start_addr = LOCCTR;
        fprintf(mid_fp, "%04X\t %s\n", LOCCTR, statement);
        printf("%d %04X %-10s %-10s %-10s\n", format, LOCCTR, symbol, opcode, operand);

        LOCCTR -= 3;
        fgets(statement, sizeof(statement), asm_fp);
        token_num = sscanf(statement, "%s %s %s %s", token[0], token[1], token[2], token[3]);   
    }
    else {
        LOCCTR = 0;
    }

    
    while(strcmp(token[0], "END") != 0) {
        strcpy(symbol, "");
        strcpy(opcode, "");
        strcpy(operand, "");
        format = 3;

        if (strcmp(token[0], ".") != 0) {
            /*  not comment */

            if (token_num == 3 && op_find(token[0]) == -1) {
                /*  첫 번째 토큰이 opcode list에 존재하지 않으면서
                    token의 개수가 3개인 경우 
                    symbol이 있는 statement로 간주 */
                strcpy(symbol, token[0]);
                strcpy(opcode, token[1]);
                strcpy(operand, token[2]);

                if (sym_find(symbol) != -1) {
                    /* SYMTAB에서 symbol을 찾으면 error flag */
                    fprintf(stderr, "duplicate symbol: %s\n", symbol);
                    //return 0;
                }
                else {
                    /* 처음 보는 symbol일 경우 SYMTAB에 추가 */
                    sym_insert(symbol, LOCCTR);
                }
                //printf("%04X %10s %10s %10s\n", LOCCTR, token[0], token[1], token[2]);
            }
            else {
                /* symbol이 없는 statement일 경우 */
                strcpy(symbol, "");
                strcpy(opcode, token[0]);
                strcpy(operand, token[1]);
            }

            if(opcode[0] == '+') {
                strcpy(opcode, opcode+1);
                format = 4;
            }

            if (strcmp(opcode, "WORD") == 0) {
                LOCCTR += 3;
            }
            else if (strcmp(opcode, "RESW") == 0) {
                LOCCTR += 3 * strtol(operand, NULL, 16);
            }
            else if (strcmp(opcode, "RESB") == 0) {
                LOCCTR += strtol(operand, NULL, 16);
            }
            else if (strcmp(opcode, "BYTE") == 0) {
                int len = strlen(operand);
                if (operand[0] == 'X') {
                    LOCCTR += (len-3)/2;
                }
                else if (operand[0] == 'C') {
                    LOCCTR += (len-3);
                }
            }
            else if (op_find(opcode) == -1) {
                /* directive의 경우 object code로 변환 x */
                //LOCCTR -= 3;
            }
            else {
                if (format == 4)
                    LOCCTR += 4;
                else
                    LOCCTR += 3;
            }            

            if (format == 4) {
                char plus[MX_TOKEN_LEN+1] = "+";
                strcat(plus, opcode);
                strcpy(opcode, plus);
            } 
        }

        printf("%d %04X %-10s %-10s %-10s\n", format, LOCCTR, symbol, opcode, operand);
        fgets(statement, sizeof(statement), asm_fp);
        token_num = sscanf(statement, "%s %s %s %s", token[0], token[1], token[2], token[3]);
    } 
    printf("%d %04X %-10s %-10s\n", format, LOCCTR, token[0], token[1]);


    // while (!feof(asm_fp)) {
    //     fgets(statement, sizeof(statement), asm_fp);
    //     token_num = sscanf(statement, "%s %s %s %s", token[0], token[1], token[2], token[3]);
    //     // if (token_num == 3)
    //     //     printf("%d %s %s %s\n", i++, token[0], token[1], token[2]);
    //     // else if (token_num == 2)
    //     //     printf("%d %s %s\n", i++, token[1], token[2]);
    //     if (token_num == 2)
    //         strcpy(token[0], "");
    //     printf("%d\t %s\t %s\t %s\t %s\n", i++, token[0], token[1], token[2], token[3]);

    // }
    program_len = LOCCTR - start_addr;

    fclose(asm_fp);
    fclose(mid_fp);   

    return program_len;
}
