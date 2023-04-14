#include "taskQueue.h"
int taskEnQueue(taskQueue_t *pQueue, int netfd){
    // 尾插法
    task_t * pNew = (task_t *)calloc(1,sizeof(task_t));
    pNew->netfd = netfd;
    if(pQueue->queueSize == 0){
        pQueue->pFront = pNew;
        pQueue->pRear = pNew;
        printf("taskEnQueue size 0\n");
    }
    else{
        pQueue->pRear->pNext = pNew;
        pQueue->pRear = pNew;
    }
    ++pQueue->queueSize;
    return 0;
}
int taskDeQueue(taskQueue_t *pQueue){
    // 头部删除法
    // 假设队列中有至少一个结点
    task_t *pCur = pQueue->pFront;
    pQueue->pFront = pCur->pNext;
    free(pCur);
    --pQueue->queueSize;
    return 0;
}

