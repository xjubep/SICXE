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

/* label 일치하는 symbol의 LOCCTR 반환하는 함수, 없으면 -1 반환 */
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
    sym_node_ptr all_list[1000];
    int cnt = 0;

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (sym_table[i] != NULL) {
            sym_node_ptr cur = sym_table[i];

            while (cur != NULL) {
                sym_node_ptr new_node = (sym_node_ptr)malloc(sizeof(SYM_NODE));

                strcpy(new_node->label, cur->label);
                new_node->LOCCTR = cur->LOCCTR;
                new_node->next = NULL;
                all_list[cnt] = new_node;

                cur = cur->next;
                cnt++;
            }
        }
    }
    qsort(all_list, cnt, sizeof(sym_node_ptr), sym_compare);
    for (int i = 0; i < cnt; i++) {
        printf("%7s %-10s %04X\n", "", all_list[i]->label, all_list[i]->LOCCTR);
    }
    return 1;
}

/* qsort함수에 사용될 정렬 함수 */
int sym_compare(const void *a, const void *b) {
    sym_node_ptr n1 = *(sym_node_ptr *)a;
    sym_node_ptr n2 = *(sym_node_ptr *)b;
    return strcmp(n1->label, n2->label);
}

/* symbol hash table 메모리 해제하는 함수 */
void sym_clear(void) {
    sym_node_ptr del;
    for (int i = 0; i < TABLE_SIZE; i++) {
        while (sym_table[i] != NULL) {
            del = sym_table[i];
            sym_table[i] = sym_table[i]->next;
            free(del);
        }
    }
}          

/*
void pop_front(List *list) {
    if (list->size == 1) {
        free(list->front);
        list->front = NULL;
        list->back = NULL;
    }
    else {
        node_ptr del = list->front;
        list->front = list->front->next;
        free(del);
    }

    list->size--;
}
*/