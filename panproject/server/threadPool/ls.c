#include "head.h"

int ls(int netfd,const char* pathName)
{
	//netfd 文件虚拟路径
    const char* path = pathName;//虚拟路径
    DIR* pdir = opendir(path);
    struct dirent* pdirent = NULL;
    char buf[4096] = {0};
    int offset = 0;//记录sprintf位置,字符串长度
    while(1){
        pdirent = readdir(pdir);
        if(pdirent == NULL){
            break;
        }
        offset += sprintf(buf+offset, "%s  ", pdirent->d_name);
    }
	send(netfd,&offset,sizeof(offset),MSG_NOSIGNAL);
	send(netfd,buf,offset,MSG_NOSIGNAL);
	return 0;
}
