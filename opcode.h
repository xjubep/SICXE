#ifndef __OPCODE_H
#define __OPCODE_H

#define TABLE_SIZE 20
#define MX_NAME_LEN 6 // SHIFTL

typedef struct op_node* op_node_ptr;

typedef struct op_node {
    int value;
    char name[MX_NAME_LEN+1];
    int format;
    op_node_ptr next;
} OP_NODE;

// typedef struct _OP_List {
//     int size;
//     node_ptr front;
//     node_ptr back;
// } OP_List;

op_node_ptr table[TABLE_SIZE];
//char 

int hash(const char* str);
void insert(int value, const char *name);
void op_insert(int value, const char *name, int format);
int op_find(const char *name); // bool
void op_print_all(void);

#endif