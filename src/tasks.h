#ifndef TASKS_H
#define TASKS_H

struct thread_pool;
typedef struct thread_pool thread_pool_t;
typedef void (*thread_func_t)(void *arg);

thread_pool_t *thread_pool_create(size_t num);
void thread_pool_destroy(thread_pool_t *tm);
bool thread_pool_add_work(thread_pool_t *tm, thread_func_t func, void *arg);
void thread_pool_wait(thread_pool_t *tm);

typedef struct thread_pool_work {
    thread_func_t func;
    void *arg;
    struct thread_pool_work *next;
} thread_pool_work_t;

// 'work_first' and 'work_last' are 
// necessary to push and pop work objs.
struct thread_pool {
    thread_pool_work_t    *work_first;
    thread_pool_work_t    *work_last;
    pthread_mutex_t  work_mutex;
    pthread_cond_t   has_work;
    pthread_cond_t   is_working;
    size_t           threads_working_count;
    size_t           threads_count;
    bool             stop;
};

#endif // TASKS_H
