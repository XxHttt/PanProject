#include"client.h"
int TCPinit(char * argv[])
{

    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sockfd,-1,"socket");

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(atoi(argv[2]));
    addr.sin_addr.s_addr=inet_addr(argv[1]);
    int ret_connect=connect(sockfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_in));
    ERROR_CHECK(ret_connect,-1,"connect");
    return sockfd;
}

int login(int sockfd)
{

    //username password
    int username_len;
    char username[128] = {0};
    int password_len;
    char password[128] = {0};
    //loop to check if login succeed
    bzero(username, sizeof(username));
    bzero(password, sizeof(password));
    fgets(username, 128, stdin);
    username_len = (int)strlen(username);
    username[username_len - 1] = '\0';
    fgets(password, 128, stdin);
    password_len = (int)strlen(password);
    password[password_len - 1] = '\0';
    send(sockfd, &username_len,sizeof(int), MSG_NOSIGNAL);
    send(sockfd, username, username_len, MSG_NOSIGNAL);
    send(sockfd, &password_len,sizeof(int), MSG_NOSIGNAL);
    send(sockfd, password, password_len, MSG_NOSIGNAL);
    int ret;
    recv(sockfd, &ret, sizeof(int), MSG_WAITALL);
    if(ret == 0)
    {
        printf("log in succeed!\n");
        return 0;
    }

    printf("log in failed, ret = %d\n", ret);
    return -1;
}




shell_type check_shell(char *shell, const int shell_len,
                       int start,int length,
                       const char * rest,shell_type type)
{

    if(start+length==shell_len&&memcmp(shell+start,rest,length)==0)
        return type;
    //?????
    else
        return TYPE_NO_ONE;
}
shell_type judge_shell(char * shell,int len)
{

    switch(shell[0])
    {
        //cd
    case 'c':return check_shell(shell,len,1,1,"d",TYPE_CD);
             //remove rmdir
    case 'r':if(shell[1]=='e')return check_shell(shell,len,2,4,"move",TYPE_REMOVE);
                 else return check_shell(shell,len,1,4,"mdir",TYPE_RMDIR);
                 //ls
    case 'l':return check_shell(shell,len,1,1,"s",TYPE_LS);
             //gets
    case 'g':return check_shell(shell,len,1,3,"ets",TYPE_GETS);
             //puts pwd
    case 'p':if(len>=2&&shell[1]=='w')return check_shell(shell,len,2,1,"d",TYPE_PWD);
                 else return check_shell(shell,len,1,3,"uts",TYPE_PUTS);

                 //mkdir
    case 'm':return check_shell(shell,len,1,4,"kdir",TYPE_MKDIR);

    case 'e':return check_shell(shell,len,1,3,"xit",TYPE_EXIT);
    }
    return TYPE_NO_ONE;
}


int sendFile(const int netfd, const char* filename)
{
    train_t train;
    train.length = strlen(filename);
    strcpy(train.data, filename);
    send(netfd, &train, sizeof(int)+train.length, MSG_NOSIGNAL);//???????

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
    send(netfd, &train.length, sizeof(int), MSG_NOSIGNAL);//????????��
    printf("length = %d\n", train.length);
    send(netfd, &data, sizeof(long), MSG_NOSIGNAL);//????????��
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
        send(netfd, &train.length, sizeof(train.length), MSG_NOSIGNAL);//???
        send(netfd, p+totalsize, train.length, MSG_NOSIGNAL);//??????mmap?????
        totalsize += train.length;
    }

    train.length = 0;
    send(netfd, &train.length, sizeof(train.length), MSG_NOSIGNAL);
    munmap(p, statbuf.st_size);
    close(fd);

    return 0;
}

int recvn(int sockfd, void *buf, int length){
    int total = 0;//???????????????????
    char *p = (char *)buf;//????????��
    while(total < length){
        ssize_t sret = recv(sockfd,p+total,length-total,0);
        total += sret;
    }
    return total;
}

int recvFile(const int sockfd, char* downame){
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

int get_pwd(int sockfd)
{

    int length;
    char buf[4096]={0};
    recv(sockfd,&length,sizeof(length),MSG_WAITALL);
    recv(sockfd,buf,length,MSG_WAITALL);
    printf("%s\n",buf);
    return 0;
}
int get_ls(int sockfd)
{

    int length;
    char buf[4096]={0};
    recv(sockfd,&length,sizeof(length),MSG_WAITALL);
    recv(sockfd,buf,length,MSG_WAITALL);
    printf("%s\n",buf);

    return 0;
}

int get_remove(int sockfd)
{
    int ret;
    recv(sockfd,&ret,sizeof(ret),MSG_WAITALL);

    if(ret == 0)
    {
        printf("remove is success \n");
    }
    else
    {
        printf("remove is not success \n");
    }

    return 0;
}
int get_cd(int sockfd)
{
    int ret;
    recv(sockfd,&ret,sizeof(ret),MSG_WAITALL);

    if(ret >= 0)
    {
        printf("cd is success \n");
    }
    else
    {
        printf("cd is not success \n");
    }

    return 0;

}

int get_mkdir(int sockfd)
{
    int ret;
    recv(sockfd,&ret,sizeof(ret),MSG_WAITALL);

    if(ret == 0)
    {
        printf("mkdir is success \n");
    }
    else
    {
        printf("mkdir is not success \n");
    }


    return 0;

}

int get_rmdir(int sockfd)
{
    int ret;
    recv(sockfd,&ret,sizeof(ret),MSG_WAITALL);

    if(ret == 0)
    {
        printf("rmdir is success \n");
    }
    else
    {
        printf("rmdir is not success \n");
    }

    return 0;
}

