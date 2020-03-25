#define MX_CMD_LEN 21

#ifndef __LIST_H
#define __LIST_H

typedef struct Node* node_ptr;

typedef struct Node {
    char data[4*MX_CMD_LEN];
    node_ptr next;
} NODE;

typedef struct _List {
    int size;
    node_ptr front;
    node_ptr back;
} List;

void set_node(node_ptr cur, char data[4*MX_CMD_LEN]);
void del_node(List *list, node_ptr prev, node_ptr cur);
void insert_node(List *list, node_ptr cur, char data[4*MX_CMD_LEN]);

void push_back(List *list, char data[4*MX_CMD_LEN]);
void push_front(List *list, char data[4*MX_CMD_LEN]);
void pop_front(List *list);
void pop_back(List *list);

void clear(List *list);
void print_list(List list);

#endif