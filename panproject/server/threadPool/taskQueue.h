//任务队列的具体实现
#ifndef __TASKQUEUE__ //防御式声明
#define __TASKQUEUE__
#include <func.h>
typedef struct task_s {
    int netfd;
    struct task_s * pNext;
    
} task_t;
typedef struct taskQueue_s {
    task_t * pFront;
    task_t * pRear;
    int queueSize;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    
} taskQueue_t;
int taskEnQueue(taskQueue_t *pQueue, int netfd);
int taskDeQueue(taskQueue_t *pQueue);
#endif

