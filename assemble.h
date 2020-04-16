#ifndef __ASSEMBLE_H
#define __ASSEMBLE_H

#define MX_FILENAME_LEN 30
#define MX_STATEMENT_LEN 240
#define MX_TOKEN_LEN 30

typedef struct statement_info {
    int token_num;
    int format;
    char token[7][MX_TOKEN_LEN+1];
    char symbol[MX_TOKEN_LEN+1];
    char opcode[MX_TOKEN_LEN+1];
    char operand[MX_TOKEN_LEN+1];
} state_info;

int assemble(char *filename);
int pass1(char *asm_filename, char *mid_filename);
state_info get_statement_info(char *statement);

#endif