#include "load.h"
#include "memory.h"
#include "symbol.h"
#include <stdio.h>
#include <string.h>

loadmap_info map[100];
int map_idx = 0;

int loader(int cmd_num, char* obj1, char* obj2, char* obj3, unsigned int PROGADDR) {
    int i, total_len = 0;
    /* 성공하면 1, 실패하면 -1 리턴 */
    if (cmd_num < 2 || cmd_num > 4)
        return -1;

    /* 관련 변수 초기화 */
    map_idx = 0;
    es_clear();

    if (load_pass1(cmd_num, obj1, obj2, obj3, PROGADDR) == -1) {
        return -1;
    }

    if (load_pass2(cmd_num, obj1, obj2, obj3, PROGADDR) == -1) {
        return -1;
    }

    printf("%-10s %-10s\n", "Control", "Symbol");
    printf("%-10s %-10s %-10s %-10s\n", "section", "name", "Address", "Length");
    for (i = 0; i < 40; i++)
        printf("-");
    printf("\n");
    for (i = 0; i < map_idx; i++) {
        if (map[i].type == 1) {
            printf("%-10s %-10s %04X %-6s %04X\n", map[i].name, "", map[i].address, "", map[i].length);
            total_len += map[i].length;
        }
        else if (map[i].type == 2) {
            printf("%-10s %-10s %04X\n", "", map[i].name, map[i].address);
        }
    }
    for (i = 0; i < 40; i++)
        printf("-");
    printf("\n");
    printf("%33s %04X\n", "total length", total_len);

    return 1;
}

int load_pass1(int cmd_num, char* obj1, char* obj2, char* obj3, unsigned int PROGADDR) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    unsigned int CSADDR = 0x00;
    int ret1 = 1, ret2 = 1, ret3 = 1;

    // set CSADDR to PROGADDR (for first control section)
    CSADDR = PROGADDR;
    ret1 = one_load_pass1(obj1, &CSADDR);

    if (cmd_num - 1 >= 2)
        ret2 = one_load_pass1(obj2, &CSADDR);
    if (cmd_num -1 >= 3)
        ret3 = one_load_pass1(obj3, &CSADDR);

    if (ret1 == -1 || ret2 == -1 || ret3 == -1)
        return -1;
    else
        return 1;
}

int one_load_pass1(char* obj_filename, unsigned int *CSADDR) {
    char obj_buf[MX_OBJ_BUF_LEN+1];
    unsigned int CSLTH = 0x00;

    FILE *fp = fopen(obj_filename, "r");
    /* file open error 처리 */
    if (!fp) {
        fprintf(stderr, "[ERROR] file open error!\n");
        return -1;
    }
    
    // while not end of input do
    while (fgets(obj_buf, sizeof(obj_buf), fp) != NULL) {
        if (obj_buf[0] == 'E') {
            *CSADDR += CSLTH;
            break;
        }
        else if (obj_buf[0] == 'H') {
            unsigned int tmp;

            // read next input record (Header record for control section)
            // set CSLTH to control sectin length
            sscanf(obj_buf, "H%6s %06X%06X", map[map_idx].name, &tmp, &map[map_idx].length);
            map[map_idx].type = 1;  // control section
            map[map_idx].address = *CSADDR;
            CSLTH = map[map_idx].length;

            // search ESTAB for control section name
            if (es_find(map[map_idx].name) != -1) {
                // set error flag (duplicate external symbol)
                fprintf(stderr, "[ERROR] duplicate external symbol!\n");
            }
            else {
                // enter control sectio name into ESTAB with value CSADDR
                es_insert(map[map_idx].name, map[map_idx].address);
            }
            map_idx++;
        }
        else if (obj_buf[0] == 'D') {
            int i, record_len = strlen(obj_buf);
            record_len -= 2;

            // for each symbol in the record 
            for (i = 0; i < record_len/12 ; i++) {
                unsigned int addr;

                sscanf(obj_buf + 1 + i*12, "%6s%06X", map[map_idx].name, &addr);
                map[map_idx].type = 2;  // symbol name
                map[map_idx].address = *CSADDR + addr;

                // search ESTAB for control section name
                if (es_find(map[map_idx].name) != -1) {
                    // set error flag (duplicate external symbol)
                    fprintf(stderr, "[ERROR] duplicate external symbol!\n");
                }
                else {
                    // enter control section name into ESTAB with value CSADDR
                    es_insert(map[map_idx].name, map[map_idx].address);
                }
                map_idx++;
            }
        }
    }
    fclose(fp);

    return 1;
}

int load_pass2(int cmd_num, char* obj1, char *obj2, char *obj3, unsigned int PROGADDR) {
    /* 성공하면 1, 실패하면 -1 리턴 */
    unsigned int CSADDR = 0x0000;
    //unsigned int EXECADDR = 0x0000;
    int ret1 = 1, ret2 = 1, ret3 = 1;

    // set CSADDR to PROGADDR
    // set  EXECADDR to PROGADDR
    CSADDR = PROGADDR;
    //EXECADDR = PROGADDR;
    ret1 = one_load_pass2(obj1, &CSADDR);

    if (cmd_num - 1 >= 2)
        ret2 = one_load_pass2(obj2, &CSADDR);
    if (cmd_num -1 >= 3)
        ret3 = one_load_pass2(obj3, &CSADDR);

    if (ret1 == -1 || ret2 == -1 || ret3 == -1)
        return -1;
    else
        return 1;
}

int one_load_pass2(char *obj_filename, unsigned int *CSADDR) {
    char obj_buf[MX_OBJ_BUF_LEN+1];
    unsigned int CSLTH = 0x00;
    char ref_name[6][6];

    FILE *fp = fopen(obj_filename, "r");
    /* file open error 처리 */
    if (!fp) {
        fprintf(stderr, "[ERROR] file open error!\n");
        return -1;
    }
    
    // while not end of input do
    while (fgets(obj_buf, sizeof(obj_buf), fp) != NULL) {
        if (obj_buf[0] == 'E') {
            *CSADDR += CSLTH;
            break;
        }
        else if (obj_buf[0] == 'H') {
            unsigned int tmp, length;
            char name[7];

            // read next input record (Header record)
            // set CSLTH to control sectin length
            sscanf(obj_buf, "H%6s %06X%06X", name, &tmp, &length);
            CSLTH = length;
            // ref_name[1]에 program 이름 저장
            strcpy(ref_name[1], name);
        }
        else if (obj_buf[0] == 'T') {
            // if object code is in character form, convert into internal representation
            // move object code from record to location (CSADDR + specified address)
            unsigned int start_addr, len, val;
            unsigned int i;

            sscanf(obj_buf, "T%06X%02X", &start_addr, &len);
            for (i = 0; i < len; i++) {
                sscanf(obj_buf + 1 + 8 + i*2, "%02X", &val);
                edit_uint(*CSADDR + start_addr + i, val);
            }
        }
        else if (obj_buf[0] == 'M') {
            unsigned int addr, len, ref_num;
            char op;
            // search ESTAB for modifying symbol name
            sscanf(obj_buf, "M%06X%02X%1c%02X", &addr, &len, &op, &ref_num);

            if (es_find(ref_name[ref_num]) != -1) {
                // if found, add/subtract symbol value at location (CSADDR + specified addr)
                // ref하는 실제 주소(ex. PROGC start addr+LISTC)를 ref_name에 더한다
                // estab에 이미 실제 주소 저장되어 있음
                unsigned int origin_addr, ref_addr, modified_addr, modified_addr_token[3];

                origin_addr = (mem[*CSADDR + addr] << 16) + (mem[*CSADDR + addr + 1] << 8) + mem[*CSADDR + addr + 2];
                ref_addr = es_find(ref_name[ref_num]); 

                if (op == '+') {
                    modified_addr = origin_addr + ref_addr;
                    modified_addr_token[0] = modified_addr >> 16;
                    if (modified_addr_token[0] > 0xFF)
                        modified_addr_token[0] = 0x00;
                }
                else if (op == '-') {
                    modified_addr = origin_addr - ref_addr;
                    if (origin_addr < ref_addr) {
                        modified_addr_token[0] = 0xFF;
                    }
                    else {
                        modified_addr_token[0] = modified_addr >> 16;
                    }
                }

                modified_addr_token[1] = (modified_addr>>8) - ((modified_addr>>16)<<8);
                modified_addr_token[2] = modified_addr - ((modified_addr>>8)<<8);

                edit_uint(*CSADDR + addr, modified_addr_token[0]);
                edit_uint(*CSADDR + addr + 1, modified_addr_token[1]);
                edit_uint(*CSADDR + addr + 2, modified_addr_token[2]);
            }
            else {
                // else, set error flag (undefined external symbol)
                fprintf(stderr, "[ERROR] undefined external symbol!\n");
            }    
        }
        else if (obj_buf[0] == 'R') {
            // ex R02LISTB 03ENDB  04LISTC 05ENDC
            char tmp_name[6];
            int ref_idx, i, record_len = strlen(obj_buf);           
            record_len -= 2; // 'R', '\n'

            sscanf(obj_buf, "R%02X%6s", &ref_idx, tmp_name);
            strcpy(ref_name[ref_idx], tmp_name);
            for (i = 0; i < record_len/8; i++) {
                sscanf(obj_buf + 1 + 8 + i*8, "%02X%6s", &ref_idx, tmp_name);
                strcpy(ref_name[ref_idx], tmp_name);
            }
        }
        else {
            continue;
        }
    }
    fclose(fp);

    return 1;
}