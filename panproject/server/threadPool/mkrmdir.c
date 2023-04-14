#include "head.h"
#include <func.h>

int mkDir(const char*pathname){
    int ret=mkdir(pathname,0777);
    printf("make dir in func\n");
    printf("ret  = %d\n", ret);
    return ret;
}

int rmDir(const char*pathname){
    int ret=rmdir(pathname);
    return ret;
}
