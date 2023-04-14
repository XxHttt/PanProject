#include "threadPool.h"
int exitPipe[2];
void handler(int signum){
    printf("signum = %d\n", signum);
    write(exitPipe[1],"1",1);
}
int main(int argc, char **argv){
    // ./server 192.168.118.128 1234 3
    ARGS_CHECK(argc,2);
    // read arguments from .conf;
    char ip[16];
    char port[8];
    char num[4];
    FILE *fp = fopen(argv[1], "r");
    fscanf(fp, "%s %s %s", ip, port, num);
    fclose(fp);
    printf("ip = %s, port = %s, num = %s\n", ip, port, num);
    pipe(exitPipe);
    if(fork() != 0){ //父进程的故事
        printf("pid = %d\n", getpid());
        close(exitPipe[0]);
        signal(SIGUSR1,handler);
        wait(NULL);
        exit(0);
    }
    // 子进程额外需要监听exitPipe的读端
    close(exitPipe[1]);
    // 准备线程池的数据结构
    threadPool_t threadPool;
    int workernum = atoi(num);
    printf("workernum = %d\n", workernum);
    threadPoolInit(&threadPool,workernum);
    // 创建子线程
    makeWorker(&threadPool);
    int sockfd;//初始化tcp连接
    tcpInit(ip,port,&sockfd);
    int epfd = epoll_create(1);
    epollAdd(epfd,sockfd);//监听sockfd
    epollAdd(epfd,exitPipe[0]);
    struct epoll_event readyset[2];
    while(1){
        int readynum = epoll_wait(epfd,readyset,2,-1);
        for(int i = 0; i < readynum; ++i){
            if(readyset[i].data.fd == sockfd){
                int netfd = accept(sockfd,NULL,NULL);
                pthread_mutex_lock(&threadPool.taskQueue.mutex);
                taskEnQueue(&threadPool.taskQueue, netfd);
                printf("main thread send a task!\n");
                pthread_cond_signal(&threadPool.taskQueue.cond);
                pthread_mutex_unlock(&threadPool.taskQueue.mutex);
            }
            else if(readyset[i].data.fd == exitPipe[0]){
                printf("threadPool is going to exit!\n");
                //for(int j = 0; j < workernum; ++j){
                //    pthread_cancel(threadPool.tidArr[j]);
                //}
                pthread_mutex_lock(&threadPool.taskQueue.mutex);
                threadPool.exitFlag = 1;//修改标志，表示将要退出
                pthread_cond_broadcast(&threadPool.taskQueue.cond);
                pthread_mutex_unlock(&threadPool.taskQueue.mutex);
                for(int j = 0; j < workernum; ++j){
                    pthread_join(threadPool.tidArr[j],NULL);
                }
                printf("main thread is closing!\n");
                exit(0);
            }
        }
    }
}

