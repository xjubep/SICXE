/* 명령어 한 개(ex. 'dump 14, 37'에서의 'dump', '14', '37')의 최대 길이 */
#define MX_CMD_LEN 21

#ifndef __LIST_H
#define __LIST_H

typedef struct Node* node_ptr;

typedef struct Node {
    char data[4*MX_CMD_LEN];    // command line에 대한 데이터
    node_ptr next;
} NODE;

/* NODE를 원소로 가지는 list */
typedef struct _List {
    int size;
    node_ptr front;
    node_ptr back;
} List;

/* 현재 프로젝트에서는 사용되지 않으나 리스트 관리를 위한 함수 */
void set_node(node_ptr cur, char data[4*MX_CMD_LEN]);
void del_node(List *list, node_ptr prev, node_ptr cur);
void insert_node(List *list, node_ptr cur, char data[4*MX_CMD_LEN]);

/* 리스트 push, pop, clear, print 함수 */
void push_back(List *list, char data[4*MX_CMD_LEN]);
void push_front(List *list, char data[4*MX_CMD_LEN]);
void pop_front(List *list);
void pop_back(List *list);

void clear(List *list);
void print_list(List list);

#endif
