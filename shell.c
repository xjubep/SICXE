#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

/* Shell에서 실행 가능한 모든 명령어들의 리스트를 화면에 출력하는 함수 */
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
    printf("assemble filename\n");
    printf("type filename\n");
    printf("symbol\n");
}

/*  
    현재 디렉토리에 있는 파일들을 출력하는 함수
    실행파일은 파일 이름 옆에 '*' 표시
    디렉토리는 파일 이름 옆에 '/' 표시  
*/
void dir(void) {
    struct dirent **namelist;   // 파일 목록이 저장될 변수
    int count;                  // scandir 함수의 리턴 값을 저장하는 변수
    char *path = ".";           // 파일 목록을 얻을 디렉토리에 대한 경로를 현재 경로(.)로 설정
    int i;

    /*  
        scandir 함수: 
        path 변수에 저장된 디렉토리에 대한 파일 목록을 얻어 (path)
        filter없이 파일 전체가 (NULL)
        d_name 기준으로 알파벳 순으로 정렬되어 (alphasort)
        namelist에 저장됨   (namelist)
        파일 목록의 개수 리턴, 오류 발생시에는 -1 리턴 
    */    
    if ((count = scandir(path, &namelist, NULL, alphasort)) == -1) {
        fprintf(stderr, "%s directory scan error\n", path);
        return;
    }

    /* '.'과 '..'은 표시하지 않기 index 2 부터 시작 */
    for (i = 2; i < count; i++) {
        char tmp_name[26];  // 파일 이름이 임시로 저장되는 변수
        struct stat sb;     // 파일 정보를 저장하는 구조체

        strcpy(tmp_name, namelist[i]->d_name);
        stat(tmp_name, &sb);

        if (namelist[i]->d_type == 4)  // 디렉토리의 경우 '/' 추가
            strcat(tmp_name, "/");        
        else if (S_IXUSR & sb.st_mode)  // 실행파일의 경우 '*' 추가
            strcat(tmp_name, "*");

        printf("%-25s", tmp_name);
        if (i%4 == 1 || i == count-1)
            printf("\n");
    }
    
    /* namelist 메모리 해제 */
    for (i = 0; i < count; i++)
        free(namelist[i]);
    free(namelist);

    return;
}

/* shell sicsim을 종료하는 함수 */
void quit(void) {
    exit(0);
}

/* filename에 해당하는 파일을 현재 디렉토리에서 읽어서 화면에 출력함 */
int type(char *filename) {
    char buf[1024];
    FILE *fp = fopen(filename, "r");

    if (!fp) {
        return 0;        
    }

    while (fgets(buf, sizeof(buf), fp))
        fputs(buf, stdout);
    
    fclose(fp);

    return 1;
}
