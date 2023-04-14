#include "head.h"

int changeDir(const char* path){
    
    int ret = open(path,O_DIRECTORY);
    return ret;
}
