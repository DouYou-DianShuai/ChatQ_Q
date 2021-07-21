#ifndef _THREADPOOL__H
#define _THREADPOOL__H


#ifdef __cplusplus
extern "C" {
#endif

#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


//任务（用户需求）
struct Task{

    void (*func)(void *arg);
    void *arg;
};

struct ThreadPool{


    struct Task *taskQ;
    int quCapacity;   //容量
    int quCount;   //计数，目前的任务数
    int quFront;  //存在前
    int quRear; //取在后

    //工作者(消费者)线程
    pthread_t *threadWorker;



    //线程管理者 
    pthread_t *threadManager;

    int minNum;
    int maxNum;
    int busyNum;//忙线程个数
    int liveNum;//存活得线程个数
    int exitNum; 

    //互斥锁
    pthread_mutex_t mutexpool;
    pthread_mutex_t mutexbusy; //提高效率

    //条件变量
    pthread_cond_t isFull;
    pthread_cond_t isEmpty;

    //销毁线程池的标志为  为0代表运行
    int shutdown;


};

#define ADDNUM 2
#define DELNUM 2

typedef struct ThreadPool ThreadPool_t;

//线程池的初始化接口
int threadPoolCreate(ThreadPool_t *pool , int min , int max, int quCapacity);

//给队列添加任务的接口
int threadPoolAdd(ThreadPool_t *pool, void(*func)(void*),void* arg);

//工作者（消费者）线程的回调函数
void *worker(void *arg);

//管理者线程的回调函数
void *manager(void *arg);

//对线程池的销毁，回收资源的接口
int threadPoolDestroy(ThreadPool_t *pool);

#ifdef __cplusplus
}
#endif

#endif