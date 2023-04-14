//每个子线程的信息
//任务队列
#ifndef __THREADPOOL__
#define __THREADPOOL__
#include <func.h>
#include "taskQueue.h"
typedef struct threadPool_s {
    pthread_t *tidArr;
    int threadnum;
    taskQueue_t taskQueue;
    int exitFlag; // 不退出写0 退出写1
} threadPool_t;
int threadPoolInit(threadPool_t *threadPool, int workernum);
int makeWorker(threadPool_t *threadPool);
int tcpInit(const char *ip, const char *port, int *psockfd);
int epollAdd(int epfd, int fd);
int epollDel(int epfd, int fd);
int transFile(int netfd);
int sendcmd(int netfd);
#endif


