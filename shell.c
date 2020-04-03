#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

void help(void) {
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}

void dir(void) {
    struct dirent **namelist;
    int count;
    char *path = "."; // 현재 경로(.)

    if ((count = scandir(path, &namelist, NULL, alphasort)) == -1) {
        // scandir (오류 발생하면 -1 리턴, 아니면 namelist에 저장된 struct dirent * 개수 리턴)
        // alphasort (d_name 기준으로 알파벳 순으로 정렬)
        fprintf(stderr, "%s directory scan error\n", path);
        return;
    }

    // .과 ..은 표시 안함
    for (int i = 2; i < count; i++) {
        char tmp_name[26];
        struct stat sb;

        strcpy(tmp_name, namelist[i]->d_name);
        stat(tmp_name, &sb);

        if (namelist[i]->d_type == 4)  // 파일이 디렉토리(d_type:4)일 경우
            strcat(tmp_name, "/");        
        else if (S_IXUSR & sb.st_mode)
            strcat(tmp_name, "*");

        printf("%-25s", tmp_name);
        if (i%4 == 1 || i == count-1)
            printf("\n");
    }
    
    // 메모리 해제
    for (int i = 0; i < count; i++)
        free(namelist[i]);
    free(namelist);

    return;
}

void quit(void) {
    exit(0);
}