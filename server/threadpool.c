#include "threadpool.h"



int threadPoolCreate(ThreadPool_t *pool , int min , int max, int quCapacity)
{
    int isError = 0;
    do{

        if(pool == NULL)
        {
            perror("fail pool is 'NULL'");
            isError = -1;
            break; 
        }
        memset(pool, 0, sizeof(ThreadPool_t));//初始化线程池结构体变量

        if(min<1) min = 1; //
        if(quCapacity<1) quCapacity = 1;
        
        //初始化线程队列
        pool->threadWorker = (pthread_t*)malloc(sizeof(pthread_t)*max);

        if(pool->threadWorker== NULL)
        {
            perror("fail to pool->threadWorker molloc error");
            isError = -2;
            break;
        }
        memset(pool->threadWorker, 0, sizeof(pthread_t)*max);
        


        pool->minNum = min;
        pool->maxNum = max;
        pool->busyNum = 0;
        pool->liveNum = min;
        pool->exitNum = 0;

        //初始化任务队列
        pool->quCapacity = quCapacity;
        pool->taskQ = (struct Task*)malloc(sizeof(struct Task)*pool->quCapacity);
        if(pool->taskQ == NULL)
        {
                perror("fail to pool->taskQ error");
                isError = -4;
                break;
            
        }
        memset(pool->taskQ, 0, sizeof(struct Task)*pool->quCapacity);
        pool->quCount = 0;
        pool->quFront = 0;
        pool->quRear  = 0;

        //初始化互斥锁和条件变量
        if(pthread_mutex_init(&pool->mutexbusy,NULL) != 0 ||
        pthread_mutex_init(&pool->mutexpool,NULL) != 0 ||
        pthread_cond_init(&pool->isEmpty,NULL) != 0 ||
        pthread_cond_init(&pool->isFull,NULL) != 0)
        {
            perror("fail to pthread_mutex_cond error");
            isError = -6;
            break;
        }
        //初始化线程销毁标志
        pool->shutdown = 0;


         for(int i = 0; i < min ; i++)
        {
            if(pthread_create(&pool->threadWorker[i],NULL,worker,pool) != 0)
            {
                perror("fail to pthread_create Worker error");
                isError = -3;
                break;
            }
            printf("第%d个线程被激活\n",i+1); 
        }       

        //初始化线程管理者
        pool->threadManager = (pthread_t *)malloc(sizeof(pthread_t));
        if(pthread_create(pool->threadManager,NULL, manager, pool) != 0)
        {
                perror("fail to pool->threadManager create error");
                isError = -5;
                break;
        }



        return 0;
    }while(0);

    if(pool&&pool->threadWorker) free(pool->threadWorker);
    if(pool&&pool->taskQ)  free(pool->taskQ);
    if(pool) free(pool);

    return isError;

} //void threradPoolCreate()


int threadPoolAdd(ThreadPool_t *pool, void(*func)(void*),void* arg)
{
    if(pool == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&pool->mutexpool);
    while(pool->quCount == pool->quCapacity && pool->shutdown == 0)
    {
        pthread_cond_wait(&pool->isFull,&pool->mutexpool);
    }
    if(pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutexpool);
        return 0;
    }

    pool->taskQ[pool->quFront].func = func;
    pool->taskQ[pool->quFront].arg = arg;
    pool->quFront = (pool->quFront + 1) % pool->quCapacity; 
    pool->quCount++;

    pthread_cond_signal(&pool->isEmpty);
    pthread_mutex_unlock(&pool->mutexpool);

    return 0;
}

void threadExit(ThreadPool_t *pool)
{
    if(pool == NULL)
    {
        return;
    }
    for(int i = 0; i < pool->maxNum; i++)
    {
        if(pool->threadWorker[i] == pthread_self())
        {
            memset(&pool->threadWorker[i],0, sizeof(pthread_t));


            break;

        }
        
    }
    printf("工作者线程 %ld 死亡\n",pthread_self());

    pthread_mutex_unlock(&pool->mutexpool);
    pthread_cancel(pthread_self());


}

void *worker(void *arg)
{
    pthread_detach(pthread_self());

    ThreadPool_t *pool = (ThreadPool_t*)arg;
    while(1)
    {
        pthread_mutex_lock(&pool->mutexpool);
        while(pool->quCount == 0 && pool->shutdown == 0)
        {
            pthread_cond_wait(&pool->isEmpty,&pool->mutexpool);
            
            if(pool->exitNum > 0)
            {

                pool->exitNum--;
                if(pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    threadExit(pool);

                    pthread_exit(NULL);
                }
            }
        }

        while(pool->shutdown == 1)
        {
            // pthread_mutex_unlock(&pool->mutexpool);
            printf("sss\n");
            threadExit(pool);
            pthread_exit(NULL);

        }

        //从任务队列里取出一个任务，并解锁
        struct Task *task = (struct Task*)malloc(sizeof(struct Task));
        memset(task, 0, sizeof(struct Task));

        task->func = pool->taskQ[pool->quRear].func;
        task->arg = pool->taskQ[pool->quRear].arg;
        pool->quRear = (pool->quRear +1 ) % pool->quCapacity;
        pool->quCount--;
        pthread_cond_signal(&pool->isFull);
        pthread_mutex_unlock(&pool->mutexpool);

        printf("工作者线程 %ld 开始工作。。。\n",pthread_self());
        pthread_mutex_lock(&pool->mutexbusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexbusy);

        task->func(task->arg);

        printf("工作者线程 %ld 退出工作。。。\n",pthread_self());
        task->func = NULL;
        free(task->arg);
        task->arg = NULL;
        free(task);
        task = NULL;
        
        pthread_mutex_lock(&pool->mutexbusy);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexbusy);
    }
} //oid work（）

void *manager(void *arg)
{
    ThreadPool_t *pool = (ThreadPool_t*)arg;

    while(1)
    {
        sleep(2); 
        pthread_mutex_lock(&pool->mutexbusy);
        int busyNum =  pool->busyNum;
        pthread_mutex_unlock(&pool->mutexbusy);

        pthread_mutex_lock(&pool->mutexpool);
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexpool);

        if(pool->shutdown == 1 )
        {
            printf("管理者线程已退出\n");
            pthread_exit(NULL);
            
        }

        //添加线程
        //当前任务数>当前的线程数  ，  并且当前线程数 < 最大线程数  则添加线程 添加ADDNUM个
        if(pool->quCount >liveNum && liveNum < pool->maxNum)
        {

        pthread_mutex_lock(&pool->mutexpool);
        int addnum = 0;
        for(int i = 0 ; i < pool->maxNum ; i++)
        {
        if(pool->threadWorker[i]==0  && pool->shutdown == 0)
            {
                pthread_create(&pool->threadWorker[i],NULL,worker,pool);
                printf("工作者线程 %ld  被管理者线程赋予生命。。。\n",pool->threadWorker[i]);
                addnum++;
                pool->liveNum++;
                if(addnum == ADDNUM || pool->liveNum  == pool->maxNum)
                {
                    break;
                }

            } 


        }

        }
        pthread_mutex_unlock(&pool->mutexpool);

        //删除线程
        //工作中的线程数*2 < 存活的线程数   && 存活的线程数 > 最小线程数   则删除DELNUM个线程
        pthread_mutex_lock(&pool->mutexpool);
        int minNum = pool->minNum;
        pthread_mutex_unlock(&pool->mutexpool);
        if(busyNum*2 < liveNum   && liveNum > minNum)
        {
            pthread_mutex_lock(&pool->mutexpool);
            pool->exitNum = DELNUM;
            pthread_mutex_unlock(&pool->mutexpool);

            for(int i = 0; i < DELNUM; i++)
            {
                pthread_cond_signal(&pool->isEmpty);
            }


        }
    } 
}// void manager()
    
    


int threadPoolDestroy(ThreadPool_t *pool)
{
    if(pool == NULL)
    {
        return -1;
    }

    pool->shutdown = 1;

    pthread_join(*pool->threadManager,NULL);

        //  pthread_mutex_lock(&pool->mutexpool);
        int liveNum = pool->liveNum;
        // pthread_mutex_unlock(&pool->mutexpool);

    for(int i = 0 ; i < liveNum; i++)
    {
        pthread_cond_broadcast(&pool->isEmpty);
    }
    sleep(5);
 
    if(pool&&pool->threadWorker)
    {
        free(pool->threadWorker);
        pool->threadWorker = NULL;
    }

    if(pool&&pool->taskQ)
    {
        free(pool->taskQ);
        pool->taskQ == NULL;
    }
    if(pool&&pool->threadManager)
    {
        free(pool->threadManager);
        pool->threadManager == NULL;
    }
    sleep(1);
   //删除互斥锁和条件变量
   int a,b,c,d;
       a= pthread_mutex_destroy(&pool->mutexbusy);
        b=pthread_mutex_destroy(&pool->mutexpool);
       c= pthread_cond_destroy(&pool->isEmpty);
        d =pthread_cond_destroy(&pool->isFull);
   
    free(pool);

    return 0;

} //
    
