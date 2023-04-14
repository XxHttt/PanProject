#include "head.h"

int rmFile(const char* path){
    int ret = remove(path);
    return ret;
}
