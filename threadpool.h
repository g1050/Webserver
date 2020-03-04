#ifndef threadpool_H
#define threadpool_H
#include "ulity.h"
#include "http_connect.h"

typedef std::function<void(int)> func_p;

struct task{
   func_p p;
   int argument;
};

class Threadpool{
public:
    Threadpool(){}
public:
    static void init(int thread_number);
    static bool destroy();
    static void *threadRoutine(void *arg);
    static bool addTask(func_p func,int arg);
private:
    static pthread_mutex_t m_lock; 
    static pthread_cond_t m_cond;
    static queue<task> m_task_queue;
    static vector<pthread_t> m_threadId_vector;
    static int m_thread_number;
    static int m_queue_size;
    static bool m_shutdown;
};

#endif

