#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 문자열에 따른 해시값을 생성하는 함수 */
int sym_hash(const char* str) {
    int hash = 326;
    int c;

    while ((c = *str++)) {
        hash = (((hash << 4) + hash) + c) % TABLE_SIZE;
    }

    return hash % TABLE_SIZE;
}

/* linked list 기반의 hash table에 symbol을 삽입하는 함수 */
void sym_insert(const char *label, unsigned int LOCCTR) {
    sym_node_ptr new_node = (sym_node_ptr)malloc(sizeof(SYM_NODE));

    strcpy(new_node->label, label);
    new_node->LOCCTR = LOCCTR;
    new_node->next = NULL;

    int idx = sym_hash(label);   

    if (sym_table[idx] == NULL)
        sym_table[idx] = new_node;
    else {        
        new_node->next = sym_table[idx];
        sym_table[idx] = new_node;
    }
}

/* label 일치하는 symbol 존재하는지 확인하는 함수 */
int sym_find(const char *label) {
    int idx = sym_hash(label);
    sym_node_ptr cur = sym_table[idx];

    while (cur != NULL) {
        if (strcmp(cur->label, label) == 0) {
            return cur->LOCCTR;
        }
        cur = cur->next;
    }
    return -1;
}

/* 전체 symbol list 출력 */
int sym_print_all(void) {
    //sym_node_ptr all_list[1000] = {0, };

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (sym_table[i] != NULL) {
            printf("%2d: ", i);
            sym_node_ptr cur = sym_table[i];
            while (cur->next != NULL) {
                printf("[%s, %04X] -> ", cur->label, cur->LOCCTR);
                cur = cur->next;
            }
            printf("[%s, %04X]\n", cur->label, cur->LOCCTR);
        }
    }
    return 1;
}

/*  assemble 과정 중에 생성된 symbol table을 화면에 출력함
    symbol table은 각자 알아서 설계함
    가장 최근에 assemble한 파일의 symbol table을 출력함
    symbol의 출력은 symbol을 기준으로 알파벳 내림차순으로 정렬
*/