#ifndef _FILENAME_
#define _FILENAME_
#include <cstdlib>

char * get_file_name(char *file_name, const char* head, int *a, int n, const char * tail) {
    char num[10];
    strcpy(file_name, head);
    for (int i = 0; i < n; ++i) {
        strcat(file_name, "_");
        //itoa(a[i], num, 10);
        int len = 0;
        while(a[i]>0) {
            num[len++] = a[i]%10+'0';
            a[i]/=10;
        }
        num[len] = '\0';
        for (int j = 0; j < len/2; ++j) {
            char tmp = num[j];
            num[j] = num[len-1-j];
            num[len-1-j] = tmp;
        }
        strcat(file_name, num);
    }
    strcat(file_name, tail);
    return file_name;
}

char * get_file_name(char *file_name, const char* head, int n, const char * tail) {
    int a[1] = {n};
    return get_file_name(file_name, head, a, 1, tail);
}

char * get_file_name(char *file_name, const char* head, int n1, int n2, const char * tail) {
    int a[2] = {n1, n2};
    return get_file_name(file_name, head, a, 2, tail);
}

char * get_file_name(char *file_name, const char* head, int n1, int n2, int n3, const char * tail) {
    int a[3] = {n1, n2, n3};
    return get_file_name(file_name, head, a, 3, tail);
}

char * get_file_name(char *file_name, const char* head, const char * mid,  const char * tail) {
    strcpy(file_name, head);
    strcat(file_name, mid);
    strcat(file_name, tail);
    return file_name;
}
#endif
