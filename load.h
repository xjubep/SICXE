#ifndef __LOAD_H
#define __LOAD_H

#define MX_OBJ_BUF_LEN 150
#define MX_OBJ_INFO_NAME_LEN 6

typedef struct load_map_info {
    int type;  // control section: 1, symbol name: 2
    char name[MX_OBJ_INFO_NAME_LEN+1];  
    unsigned int address;
    unsigned int length;    // control section
} loadmap_info;

int loader(int cmd_num, char* obj1, char* obj2, char* obj3, unsigned int PROGADDR);
int load_pass1(int cmd_num, char* obj1, char* obj2, char* obj3, unsigned int PROGADDR);
int load_pass2(int cmd_num, char* obj1, char *obj2, char *obj3, unsigned int PROGADDR);
int one_load_pass1(char* obj_filename, unsigned int *CSADDR);
int one_load_pass2(char* obj_filename, unsigned int *CSADDR);

#endif
