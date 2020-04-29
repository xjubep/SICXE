#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void set_node(node_ptr cur, char data[4*MX_CMD_LEN]) {
    strcpy(cur->data, data);
}

void del_node(List *list, node_ptr prev, node_ptr cur) {
    if (prev == NULL) {
        cur = cur->next;
        free(list->front);
        list->front = cur;
    }
    else {
        prev->next = cur->next;
        free(cur);
    }

    list->size--;
}

void insert_node(List *list, node_ptr cur, char data[4*MX_CMD_LEN]) {
    node_ptr new_node = (node_ptr)malloc(sizeof(NODE));
    strcpy(new_node->data, data);
    new_node->next = cur->next;
    cur->next = new_node;

    list->size++;
}


void push_back(List *list, char data[4*MX_CMD_LEN]) {
    node_ptr new_node = (node_ptr)malloc(sizeof(NODE));
    strcpy(new_node->data, data);
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

void push_front(List *list, char data[4*MX_CMD_LEN]) {
    node_ptr new_node = (node_ptr)malloc(sizeof(NODE));
    strcpy(new_node->data, data);
    new_node->next = NULL;

    if (list->size == 0) {
        list->front = new_node;
        list->back = new_node;
    }
    else {
        new_node->next = list->front;
        list->front = new_node;
    }

    list->size++;
}

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

void pop_back(List *list) {
    if (list->size == 1) {
        free(list->back);
        list->front = NULL;
        list->back = NULL;
    }
    else {
        node_ptr prev, cur;
        prev = NULL;
        for (cur = list->front; cur->next != NULL; cur = cur->next)
            prev = cur;
        free(cur);
        prev->next = NULL;
        list->back = prev;
    }

    list->size--;
}

void clear(List *list) {
    while(list->size)
        pop_back(list);
}

void print_list(List list) {
    node_ptr cur;
    for (cur = list.front; cur != NULL; cur = cur->next)
        printf("%s ", cur->data);
    printf("\n");
}
