#ifndef __SYMBOL_H
#define __SYMBOL_H

#define TABLE_SIZE 20   // hash table size=20
#define MX_SYM_LEN 10   // symbol 최대 길이 

typedef struct sym_node* sym_node_ptr;

/* symbol node 구조체 */
typedef struct sym_node {       
    char label[MX_SYM_LEN+1];   
    unsigned int LOCCTR;                   
    sym_node_ptr next;
} SYM_NODE;

sym_node_ptr sym_table[TABLE_SIZE];  // symbol를 저장할 hash table 포인터 변수

int sym_hash(const char* str);      // 문자열에 따른 해시값을 생성하는 함수
void sym_insert(const char *label, unsigned int LOCCTR); // linked list 기반의 hash table에 symbol를 삽입하는 함수
int sym_find(const char *label);  // label과 일치하는 symbol가 존재하는지 확인하는 함수
int sym_print_all(void);        // 전체 symbol list 출력

#endif