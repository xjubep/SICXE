#include "run.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int bp(int cmd_num, char* addr, BP_List *bl) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    if (cmd_num > 2)
        return -1;

    if  (cmd_num == 1) {
        // print all bp
        int i;

        printf("%10s %10s\n", "", "breakpoint");
        printf("%10s ", "");
        for (i = 0; i < 10; i++)
            printf("-");
        printf("\n");
        bp_print_list(*bl);
    }
    else if (strcmp(addr, "clear") == 0) {
        // ex. bp clear
        bp_clear(bl);
        printf("%10s [ok] clear all breakpoints\n", "");
    }
    else {
        // ex. bp 3
        bp_push_back(bl, addr);
        printf("%10s [ok] create breakpoint %s\n", "", addr);
    }

    return 1;
}

/* bp list 연산*/
void bp_push_back(BP_List *list, char addr[5]) {
    bp_node_ptr new_node = (bp_node_ptr)malloc(sizeof(BP_NODE));
    //strcpy(new_node->addr, addr);
    new_node->addr = strtoul(addr, NULL, 16);
    new_node->next = NULL;

    if (list->size == 0) {
        list->front = new_node;
        list->back = new_node;
    }
    else {
        list->back->next = new_node;
        list->back = new_node;
    }

    list->size++;
}

void bp_pop_back(BP_List *list) {
    if (list->size == 1) {
        free(list->back);
        list->front = NULL;
        list->back = NULL;
    }
    else {
        bp_node_ptr prev, cur;
        prev = NULL;
        for (cur = list->front; cur->next != NULL; cur = cur->next)
            prev = cur;
        free(cur);
        prev->next = NULL;
        list->back = prev;
    }

    list->size--;
}

void bp_clear(BP_List *list) {
    while(list->size)
        bp_pop_back(list);
}

void bp_print_list(BP_List list) {
    bp_node_ptr cur;
    for (cur = list.front; cur != NULL; cur = cur->next)
        printf("%10s %X\n", "", cur->addr);
}

int run(BP_List *bl) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    
}