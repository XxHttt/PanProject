#include "head.h"

int recvn(int sockfd, void *buf, int length){
    int total = 0;//记录已经收到的数据的长度
    char *p = (char *)buf;//以字节为单位
    while(total < length){
        ssize_t sret = recv(sockfd,p+total,length-total,0);
        total += sret;
    }
    return total;
}

int sendFile(const int netfd, const char* filename)
{
    train_t train;
    train.length = strlen(filename);
    strcpy(train.data, filename);
    send(netfd, &train, sizeof(int)+train.length, MSG_NOSIGNAL);

    int fd = open(filename, O_RDWR);
    //ERROR_CHECK(fd, -1, "openfd");
    if(fd == -1)
    {
        printf("open failed\n");
        train.length = -1;
        send(netfd, &train, sizeof(int), MSG_NOSIGNAL);
        return -1;
    }
    struct stat statbuf;
    fstat(fd, &statbuf);
    train.length = (int)sizeof(statbuf.st_size);
    long data = statbuf.st_size;
    memcpy(train.data, &statbuf.st_size, train.length);
    send(netfd, &train.length, sizeof(int), MSG_NOSIGNAL);
    printf("length = %d\n", train.length);
    send(netfd, &data, sizeof(long), MSG_NOSIGNAL);
    printf("data = %ld\n", data);
    
    off_t offset;
    recv(netfd, &offset, sizeof(offset), MSG_WAITALL);
    printf("offset = %ld\n", offset);

    char *p = (char*)mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    p = p + offset;
    ERROR_CHECK(p, MAP_FAILED, "mmap");

    off_t totalsize = 0;
    while(totalsize < statbuf.st_size)
    {

        if(statbuf.st_size - totalsize > 1000)
        {
            train.length = 1000;
        }
        else
        {
            train.length = statbuf.st_size - totalsize;
        }
        send(netfd, &train.length, sizeof(train.length), MSG_NOSIGNAL);
        send(netfd, p+totalsize, train.length, MSG_NOSIGNAL);
        totalsize += train.length;
    }

    train.length = 0;
    send(netfd, &train.length, sizeof(train.length), MSG_NOSIGNAL);
    munmap(p, statbuf.st_size);
    close(fd);

    return 0;
}


int recvFile(const int sockfd, const char* downame){
    char filename[1024] = {0};
    int length;
    recvn(sockfd,&length,sizeof(length));
    recvn(sockfd,filename,length);//收文件的名字
    off_t filesize;
    recvn(sockfd,&length,sizeof(length));
    if(length == -1)
    {
        printf("no such file in the client\n");
        return -1;
    }
    printf("length = %d\n", length);
    recvn(sockfd,&filesize,sizeof(off_t));//收文件的大小
    printf("filesize = %ld\n", filesize);
    int fd = open(downame,O_RDWR|O_CREAT,0666);
    //ERROR_CHECK(fd, -1, "open");
    if(fd == -1)
    {
        printf("open failed\n");
        return -1;
    }
    printf("fd = %d\n", fd);
    off_t offset = lseek(fd, 0, SEEK_END);//如果断，断点续收
    printf("offset = %ld\n", offset);
    train_t train;
    bzero(&train, sizeof(train));
    send(sockfd, &offset, sizeof(offset), MSG_NOSIGNAL);

    off_t cursize = offset;
    off_t slice = filesize/10000;
    off_t lastsize = 0;//上次打印的时候文件大小
    
    while(1){
        char buf[4096] = {0};
        recvn(sockfd,&length,sizeof(length));
        if(length != 1000){
            printf("length = %d\n", length);
        }
        if(length == 0){
            break;
        }
        recvn(sockfd,buf,length);
        write(fd,buf,length);
        
        cursize += length;

        if(cursize - lastsize > slice){
            printf("%5.2lf%%\r",100.0*cursize/filesize);
            fflush(stdout);
            lastsize = cursize;
        }
    }
    printf("100.00%%\n");
    close(fd);
    return 0;
}

