#ifndef __HEAD__
#define __HEAD__ 
#include <func.h>
int sendFile(const int netfd, const char* filename);
int recvn(int sockfd, void *buf, int length);
int recvFile(const int sockfd, const char* downname);
typedef struct train_s {
    int length;
    char data[1000];
} train_t;
int mkDir(const char*pathname);
int rmDir(const char*pathname);
int ls(int netfd,const char* pathName);
int changeDir(const char* path);
int rmFile(const char* path);
int pswValidation(const char*username,const char*password);
#define LOG_LOGIN(user){\
    time_t now = time(NULL);\
    syslog(LOG_INFO, "user:%s, loginTime:%s",user, ctime(&now));\
}

#define LOG_MSG(user, operation){\
    time_t now = time(NULL);\
    syslog(LOG_INFO, "SUCCEED:user:%s, operation:%s, time:%s", user, operation, ctime(&now));\
}

#define LOG_ERROR(user, operation){ \
    syslog(LOG_INFO, "ERROR: user:%s, operation:%s, line:%d, function:%s, file:%s",user,operation,__LINE__,__FUNCTION__,__FILE__);\
}


#endif
