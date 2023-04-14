#pragma once


#ifndef _CLIENT_H_
#define _CLIENT_H_
#include<func.h>

typedef enum
{
    //cd       remove
    TYPE_CD,TYPE_REMOVE,
    //ls    pwd
    TYPE_LS,TYPE_PWD,
    //mkdir       rmdir
    TYPE_MKDIR,TYPE_RMDIR,
    //gets     puts
    TYPE_GETS,TYPE_PUTS,
    //exit
    TYPE_EXIT,

    //???????
    TYPE_NO_ONE
}shell_type;

typedef struct train_s
{
    int length;
    char data[1024];
}train_t;

int login(int sockfd);

int get_pwd(int sockfd);
int get_ls(int sockfd);


int get_remove(int sockfd);
int get_cd(int sockfd);

int get_mkdir(int sockfd);

int get_rmdir(int sockfd);

int TCPinit(char * argv[]);
shell_type check_shell(char *shell, const int shell_len,
                       int start,int length,
                       const char * rest,shell_type type);
shell_type judge_shell(char * shell,int len);
int sendFile(const int netfd, const char* filename);
int recvn(int sockfd, void *buf, int length);
int recvFile(const int sockfd, char *shell);

int get_tree(int sockfd);




#endif

