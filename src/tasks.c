// Reference used to implement thread-pooling
// https://nachtimwald.com/2019/04/12/thread-pool-in-c/

#include <stdbool.h>
#include <stddef.h>
#include <pthread.h>
#include <stdlib.h>
#include "main.h"
#include "tasks.h"

static thread_pool_work_t *thread_pool_work_create(thread_func_t func, void *arg)
{
    if (func == NULL) { return NULL; }

    _Debug({ LogDebug("Creating work in thread: %li\n", pthread_self()); });

    thread_pool_work_t *work = malloc(sizeof(*work));
    work->func = func;
    work->arg = arg;
    work->next = NULL;
    return work;
}

static void thread_pool_work_destroy(thread_pool_work_t *work)
{
    if (work == NULL) { return; }

    _Debug({ LogDebug("Destroying pool work %li\n", (size_t)work); });

    free(work);
    work = NULL;
}

static thread_pool_work_t *thread_pool_work_get(thread_pool_t *tm)
{
    if (tm == NULL) { return NULL; }

    thread_pool_work_t *work = tm->work_first;
    if (work == NULL) {
        return NULL;
    }

    if (work->next == NULL) {
        tm->work_first = NULL;
        tm->work_last = NULL;
    } else {
        tm->work_first = work->next;
    }

    return work;
}

static void *thread_pool_worker(void *arg)
{
    thread_pool_t *tm = arg;
    thread_pool_work_t *work;

    _Debug({ LogDebug("Initializing pool worker (thread %li)\n", pthread_self()); });

    while (true) {
        pthread_mutex_lock(&(tm->work_mutex));

        while (tm->work_first == NULL && !tm->stop) {
            pthread_cond_wait(&(tm->has_work), &(tm->work_mutex));
        }

        if (tm->stop) {
            _Debug({ LogDebug("Stopping to work"); });
            break;
        }

        work = thread_pool_work_get(tm);
        tm->threads_working_count++;
        pthread_mutex_unlock(&(tm->work_mutex));

        if (work != NULL) {
            work->func(work->arg);
            thread_pool_work_destroy(work);
        }

        pthread_mutex_lock(&(tm->work_mutex));
        tm->threads_working_count--;
        if (!tm->stop && tm->threads_working_count == 0 && tm->work_first == NULL) {
            pthread_cond_signal(&(tm->is_working));
        }
        pthread_mutex_unlock(&(tm->work_mutex));
    }

    tm->threads_count--;
    pthread_cond_signal(&(tm->is_working));
    pthread_mutex_unlock(&(tm->work_mutex));
    return NULL;
}

thread_pool_t *thread_pool_create(size_t num)
{
    if (num < 1) num = 2;

    thread_pool_t *tm = calloc(1, sizeof(*tm));
    tm->threads_count = num;

    pthread_mutex_init(&(tm->work_mutex), NULL);
    pthread_cond_init(&(tm->has_work), NULL);
    pthread_cond_init(&(tm->is_working), NULL);

    for (size_t i = 0; i < num; ++i) {
        pthread_t thread = {0};

        pthread_create(&thread, NULL, thread_pool_worker, tm);
        pthread_detach(thread);
    }

    _Debug({ LogDebug("Thread pool created with %li threads\n", num); });

    return tm;
}

void thread_pool_destroy(thread_pool_t *tm)
{
    thread_pool_work_t *next_work;

    if (tm == NULL) return;

    pthread_mutex_lock(&(tm->work_mutex));
    thread_pool_work_t *work = tm->work_first;
    while (work != NULL) {
        next_work = work->next;
        thread_pool_work_destroy(work);
        work = next_work;
    }
    tm->stop = true;
    pthread_cond_broadcast(&(tm->has_work));
    pthread_mutex_unlock(&(tm->work_mutex));

    thread_pool_wait(tm);

    pthread_mutex_destroy(&(tm->work_mutex));
    pthread_cond_destroy(&(tm->is_working));
    pthread_cond_destroy(&(tm->has_work));

    free(tm);
    tm = NULL;
}

bool thread_pool_add_work(thread_pool_t *tm, thread_func_t func, void *arg)
{
    if (tm == NULL) {
        LogError("Could not create new work due to empty thread pool.\n")
        return false;
    }

    _Debug({ LogDebug("Adding work (thread %li)\n", pthread_self()); });
    
    thread_pool_work_t *work = NULL;
    if ((work = thread_pool_work_create(func, arg)) == NULL) {
        LogError("Could not create new work in thread pool.\n")
        return false;
    }

    pthread_mutex_lock(&(tm->work_mutex));

    if (tm->work_first == NULL) {
        tm->work_first = work;
        tm->work_last = work;
    } else {
        tm->work_last->next = work;
        tm->work_last = work;
    }

    pthread_cond_broadcast(&(tm->has_work));
    pthread_mutex_unlock(&(tm->work_mutex));

    _Debug({ LogDebug("Work added to thread pool successfully.\n"); });

    return true;
}

void thread_pool_wait(thread_pool_t *tm)
{
    if (tm == NULL) return;

    pthread_mutex_lock(&(tm->work_mutex));
    while(true) {
        if ((!tm->stop && tm->threads_working_count != 0) || (tm->stop && tm->threads_count != 0) || (tm->work_first != NULL)) {
            pthread_cond_wait(&(tm->is_working), &(tm->work_mutex));
            _Debug({ LogDebug("Pool waiting for work"); });
        } else {
            _Debug({ LogDebug("Pool found no work\n"); });
            break;
        }
    }
    pthread_mutex_unlock(&(tm->work_mutex));
}
