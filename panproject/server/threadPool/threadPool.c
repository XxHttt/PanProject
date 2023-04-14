#include "threadPool.h"
int threadPoolInit(threadPool_t *threadPool, int workernum){
    bzero(threadPool, sizeof(threadPool_t));
    threadPool->threadnum = workernum;
    threadPool->tidArr = (pthread_t *)calloc(workernum,sizeof(pthread_t));
    pthread_mutex_init(&threadPool->taskQueue.mutex,NULL);
    pthread_cond_init(&threadPool->taskQueue.cond,NULL);
    return 0;
}

