#include "threadPool.h"
#include "head.h"
void cleanmutex(void *arg){
    threadPool_t * threadPool = (threadPool_t *)arg;
    pthread_mutex_unlock(&threadPool->taskQueue.mutex);
}
void *threadFunc(void *arg){
    threadPool_t * threadPool = (threadPool_t *)arg;
    while(1){
        int netfd;
        // 准备取出任务
        printf("i am here\n");
        pthread_mutex_lock(&threadPool->taskQueue.mutex);
        while(threadPool->exitFlag == 0 && threadPool->taskQueue.queueSize == 0){
            // 队列为空，子线程陷入等待
            printf("pthread cond_wait\n");
            pthread_cond_wait(&threadPool->taskQueue.cond, &threadPool->taskQueue.mutex);
        }
        if(threadPool->exitFlag != 0){
            printf("child thread is going to exit!\n");
            pthread_mutex_unlock(&threadPool->taskQueue.mutex);
            pthread_exit(NULL);
        }
        printf("worker thread get task!\n");
        netfd = threadPool->taskQueue.pFront->netfd; // get queue top
        taskDeQueue(&threadPool->taskQueue);
        pthread_mutex_unlock(&threadPool->taskQueue.mutex);
        // send the cmd and find a suitable function
        sendcmd(netfd);

        close(netfd);
    }
}
int makeWorker(threadPool_t *threadPool){
    for(int i = 0; i < threadPool->threadnum; ++i){
        printf("i = %d\n", i);
        pthread_create(&threadPool->tidArr[i],NULL,threadFunc,threadPool);
    }
    return 0;
}

