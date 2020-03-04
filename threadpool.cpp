#include "threadpool.h"

pthread_mutex_t Threadpool::m_lock; 
pthread_cond_t Threadpool::m_cond;
queue<task> Threadpool::m_task_queue;
vector<pthread_t> Threadpool::m_threadId_vector;
int Threadpool::m_thread_number;
int Threadpool::m_queue_size;
bool Threadpool::m_shutdown = false;


void Threadpool::init(int thread_number){
    /* Init the mutex and condition variables. */
    pthread_mutex_init(&m_lock,NULL);
    pthread_cond_init(&m_cond,NULL);

    m_thread_number = thread_number;
    m_shutdown = false; 
    for(int i = 0;i<m_thread_number;i++){
        pthread_t threadid;    
        pthread_create(&threadid,NULL,threadRoutine,NULL);
        m_threadId_vector.push_back(threadid);
    }

}
void* Threadpool::threadRoutine(void *arg){

    debug("0x%lx was added to the threadpool\n",pthread_self());
    /* Infinite loop to hold all the threads. */
    while(true){
        pthread_mutex_lock(&m_lock);
        /* When no task and m_shutdown is false,all threads block. */
        while(m_task_queue.empty() && !m_shutdown){
            pthread_cond_wait(&m_cond,&m_lock);
        }

        /* Thread pool is going to be destroyed. */
        if(m_shutdown){
            pthread_mutex_unlock(&m_lock);
            debug("0x%ld was going to be destroyed\n",pthread_self());
            pthread_exit(NULL);
        }

        check_exit(!m_task_queue.empty(),"Empty task queue\n");

        task cur_task = m_task_queue.front();
        m_task_queue.pop();
        cur_task.p(cur_task.argument);
        /* Unlock */
        pthread_mutex_unlock(&m_lock);
    }
    return NULL;
}


bool Threadpool::addTask(func_p p,int argument){
    task task_add;
    task_add.p = p;
    task_add.argument = argument;

    pthread_mutex_lock(&m_lock);
    m_task_queue.push(task_add);
    pthread_mutex_unlock(&m_lock);

    /* Arouse a thread to do the worker; */
    int ret = pthread_cond_signal(&m_cond);
    if(!ret){
        return true;
    }else{
        debug("Arouse failed\n");
        return false;
    }
}

bool Threadpool::destroy(){
    if(m_shutdown){
        debug("Call destroy again\n");
    } 
    m_shutdown = true;
    pthread_cond_broadcast(&m_cond);

    for(auto threadId:m_threadId_vector){
        if(pthread_join(threadId,NULL)){
            debug("Destroy thread:0x%lx failed\n",threadId);
        }
    }

    pthread_mutex_destroy(&m_lock);
    pthread_cond_destroy(&m_cond);
    return true;
}
