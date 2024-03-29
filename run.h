#ifndef __RUN_H
#define __RUN_H

typedef struct BP_Node* bp_node_ptr;

typedef struct BP_Node {
    unsigned int addr;    // bp의 주소 값
    bp_node_ptr next;
} BP_NODE;

/* BP_NODE를 원소로 가지는 list */
typedef struct _BP_List {
    int size;
    bp_node_ptr front;
    bp_node_ptr back;
} BP_List;

typedef struct _REG {
    unsigned int A, X, L, B, S, T, F, PC, SW;
} REG;

void bp_push_back(BP_List *list, char addr[5]);
void bp_pop_back(BP_List *list);
void bp_clear(BP_List *list);
void bp_print_list(BP_List list);

int bp(int cmd_num, char* addr, BP_List *bl);

int run(BP_List *bl, REG* reg, unsigned int PROGADDR);
void init_register(REG* reg, unsigned int PROGADDR);
void print_register(REG* reg);

#endif
