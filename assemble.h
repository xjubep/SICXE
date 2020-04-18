#ifndef __ASSEMBLE_H
#define __ASSEMBLE_H

#define MX_FILENAME_LEN 30
#define MX_STATEMENT_LEN 150
#define MX_TOKEN_LEN 30

typedef struct statement_info {
    int is_comment;
    int token_num;
    int format;
    int LOCCTR;
    char token[4][MX_TOKEN_LEN+1];
    char symbol[MX_TOKEN_LEN+1];
    char opcode[MX_TOKEN_LEN+1];
    char operand[MX_TOKEN_LEN+1];
    char operand2[MX_TOKEN_LEN+1];
    char statement[MX_STATEMENT_LEN+1];
} state_info;

int assemble(char *filename);
int pass1(char *asm_filename, char *mid_filename);
int pass2(char *mid_filename, char *lst_filename, char *obj_filename);
state_info get_statement_info(char *statement);
int is_register(char *operand);

#endif