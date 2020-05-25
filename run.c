#include "run.h"
#include "memory.h"
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

int run(BP_List *bl, REG* reg, unsigned int PROGADDR) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    init_register(reg, PROGADDR);
    //print_register(reg);
    // printf("%06X %02X\n", reg->PC, mem[reg->PC]);
    // printf("%06X %02X\n", reg->PC + 1, mem[reg->PC + 1]);
    // printf("%06X %02X\n", reg->PC + 2, mem[reg->PC + 2]);
    int tc = 10;
    while (tc--) {
        unsigned int opcode = mem[reg->PC] - mem[reg->PC]%4;
        unsigned int ni = mem[reg->PC]%4; // 0(00), 1(01, immediate), 2(10, indirect), 3(11, simple)
        unsigned int xbpe = mem[reg->PC+1] >> 4; // odd: format4
        unsigned int disp = mem[reg->PC+1] - ((mem[reg->PC+1] >> 4) << 4) + mem[reg->PC+2];
        unsigned int addr;

        int format = 3;
        if (xbpe%2 == 1) {
            format = 4;
            addr = (disp << 8) + mem[reg->PC+3];
            printf("%05X\n", addr);
        }
        printf("%02X %02X %02X %03X\n", opcode, ni, xbpe, disp);

        if (opcode == 0xB4) {
            // CLEAR X CLEAR A CLEAR S
            printf("CLEAR\n");
            unsigned int r1 = mem[reg->PC+1] >> 4;
            if (r1 == 0x0) {
                reg->A = 0;
            }
            else if (r1 == 0x1) {
                reg->X = 0;
            }
            else if (r1 == 0x2) {
                reg->L = 0;
            }
            else if (r1 == 0x3) {
                reg->B = 0;
            }
            else if (r1 == 0x4) {
                reg->S = 0;
            }
            else if (r1 == 0x5) {
                reg->T = 0;
            }
            else if (r1 == 0x6) {
                reg->F = 0;
            }
            else if (r1 == 0x8) {
                reg->PC = 0;
            }
            else if (r1 == 0x9) {
                reg->SW = 0;
            }
            format = 2;
            reg->PC += format;
        }
        else if (opcode == 0x28) {
            // COMP #0
            printf("COMP\n");
            reg->PC += format;
        }
        else if (opcode == 0xA0) {
            // COMPR A, S
            printf("COMPR\n");
        }
        else if (opcode == 0x3C) {
            // J CLOOP
            printf("J\n");
            reg->PC += format;
        }
        else if (opcode == 0x30) {
            // JEQ  ENDFIL
            printf("JEQ\n");
            reg->PC += format;
        }
        else if (opcode == 0x38) {
            // JLT RLOOP
            printf("JLT\n");
        }
        else if (opcode == 0x48) {
            // +JSUB RDREC, +JSUB WRREC
            printf("JSUB\n");
            reg->PC += format;
        }
        else if (opcode == 0x00) {
            // LDA LENGTH
            printf("LDA\n");
            reg->PC += format;
        }
        else if (opcode == 0x68) {
            // LDB #LENGTH
            printf("LDB\n");
            //
            reg->PC += format;
        }
        else if (opcode == 0x50) {
            // LDCH BUFFER, X
            printf("LDCH\n");
        }
        else if (opcode == 0x74) {
            // +LDT #4096
            printf("LDT\n");
        }
        else if (opcode == 0xD8) {
            // RD INPUT
            printf("RD\n");
        }
        else if (opcode == 0x4C) {
            // RSUB
            printf("RSUB\n");
        }
        else if (opcode == 0x0C) {
            // STA BUFFER
            printf("STA\n");
            reg->PC += format;
        }
        else if (opcode == 0x54) {
            // STCH BUFFER, X
            printf("STCH\n");
        }
        else if (opcode == 0x14) {
            // STL RETADR
            // mem[reg->PC]%4 n, i 구할 수 있음
            printf("STL\n");
            unsigned int target_addr;
            if (ni == 0b11 && xbpe == 0b0010) {
                // simple addressing && pc relative
                target_addr = reg->PC + disp;
                printf("target: %04X\n", target_addr);
            }
            //reg->L = 
            reg->PC += format;
        }
        else if (opcode == 0x10) {
            // STX LENGTH
            printf("STX\n");
        }
        else if (opcode == 0xE0) {
            // TD INPUT
            printf("TD\n");
        }
        else if (opcode == 0xB8) {
            // TIXR T
            printf("TIXR\n");
        }
        else if (opcode == 0xDC) {
            // WD OUTPUT
            printf("WD\n");
        }
        else {
            //printf("[ERROR] undefined opcode\n");
        }
    }


    return 1;
}

void init_register(REG* reg, unsigned int PROGADDR) {
    reg->A = 0;
    reg->X = 0;
    reg->L = 0;
    reg->B = 0;
    reg->S = 0;
    reg->T = 0;
    reg->F = 0;
    reg->PC = PROGADDR;
    reg->SW = 0;
}

void print_register(REG* reg) {
    printf("%2s : %06X %2s : %06X\n", "A", reg->A, "X", reg->X);
    printf("%2s : %06X %2s : %06X\n", "L", reg->L, "PC", reg->PC);
    printf("%2s : %06X %2s : %06X\n", "B", reg->B, "S", reg->S);
    printf("%2s : %06X\n", "T", reg->T);
}