#include "opcode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int hash(const char* str) {
    int hash = 326;
    int c;

    while ((c = *str++)) {
        hash = (((hash << 4) + hash) + c) % TABLE_SIZE;
    }

    return hash % TABLE_SIZE;
}

void op_insert(int value, const char *name, int format) {
    op_node_ptr new_node = (op_node_ptr)malloc(sizeof(OP_NODE));

    new_node->value = value;
    strcpy(new_node->name, name);
    new_node->format = format;
    new_node->next = NULL;

    int idx = hash(name);

    if (table[idx] == NULL)
        table[idx] = new_node;
    else {
        op_node_ptr cur = table[idx];

        while (cur != NULL) {
            // name이 중복일 경우 value 값을 바꾼다.
            // 그러나 현재 플젝에서는 필요 없는 거 같음
            if (strcmp(cur->name, name) == 0) {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }

        // 중복이 아닐 경우 push_front
        new_node->next = table[idx];
        table[idx] = new_node;
    }
}

int op_find(const char *name) {
    int idx = hash(name);
    op_node_ptr cur = table[idx];

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            printf("opcode is %02X\n", cur->value);
            return 1;
        }
        cur = cur->next;
    }
    return 0;
}

void op_print_all(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
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