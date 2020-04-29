#ifndef __OPCODE_H
#define __OPCODE_H

#define TABLE_SIZE 20   // hash table size=20
#define MX_NAME_LEN 6   // opcode 명령어의 최대 길이 (SHIFTL 명령어)

typedef struct op_node* op_node_ptr;

/* opcode node 구조체 */
typedef struct op_node {        // 18 ADD 3/4
    int value;                  // 18
    char name[MX_NAME_LEN+1];   // ADD
    int format;                 // '1':0, '2':1, '3/4':2 저장
    op_node_ptr next;
} OP_NODE;

op_node_ptr table[TABLE_SIZE];  // opcode를 저장할 hash table 포인터 변수

int hash(const char* str);      // 문자열에 따른 해시값을 생성하는 함수
void op_insert(int value, const char *name, int format); // linked list 기반의 hash table에 opcode를 삽입하는 함수
op_node_ptr op_find(const char *name);  // instruction name과 일치하는 ocpode가 존재하는지 확인하는 함수
void op_print_all(void);        // 전체 opcodelist 출력

#endif
