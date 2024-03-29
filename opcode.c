#include "opcode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 문자열에 따른 해시값을 생성하는 함수 */
int hash(const char* str) {
    int hash = 326;
    int c;

    while ((c = *str++)) {
        hash = (((hash << 4) + hash) + c) % TABLE_SIZE;
    }

    return hash % TABLE_SIZE;
}

/* linked list 기반의 hash table에 opcode를 삽입하는 함수 */
void op_insert(int value, const char *name, int format) {
    op_node_ptr new_node = (op_node_ptr)malloc(sizeof(OP_NODE));

    new_node->value = value;
    strcpy(new_node->name, name);
    new_node->format = format;
    new_node->next = NULL;

    /* 문자열(ex. ADD)를 기반으로 해시값 생성하고 그에 따른 hash table의 인덱스 지정함 */
    int idx = hash(name);   

    if (table[idx] == NULL)
        table[idx] = new_node;
    else {        
        new_node->next = table[idx];
        table[idx] = new_node;
    }
}

/*  instruction name과 일치하는 ocpode가 존재하는지 확인하고 
    존재하면 해당 opcode의 정보를 알려주는 함수 */
op_node_ptr op_find(const char *name) {
    int idx = hash(name);
    op_node_ptr cur = table[idx];

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

/* 전체 opcodelist 출력 */
void op_print_all(void) {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        if (table[i] != NULL) {
            printf("%2d: ", i);
            op_node_ptr cur = table[i];
            while (cur->next != NULL) {
                printf("[%s, %02X] -> ", cur->name, cur->value);
                cur = cur->next;
            }
            printf("[%s, %02X]\n", cur->name, cur->value);
        }
    }

}
