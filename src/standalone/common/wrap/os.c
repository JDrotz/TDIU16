#include "os.h"
#include "thread.h"
#include "synch.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct thread_start {
    thread_func *run;
    void *aux;
    struct semaphore started;
};

static void thread_main(struct thread_start *start) {
    thread_func *run = start->run;
    void *aux = start->aux;

    sema_up(&start->started);

    // From here on, we can not touch anything inside 'start'. It might be gone by now!
    (*run)(aux);
}

#ifdef POSIX

static void *pthread_main(void *data) {
    thread_main(data);
    return NULL;
}

tid_t thread_create(const char *name, int priority, thread_func *fn, void *aux) {
    struct thread_start info;
    info.run = fn;
    info.aux = aux;
    sema_init(&info.started, 0);

    UNUSED(name);
    UNUSED(priority);

    pthread_t created;
    if (pthread_create(&created, NULL, &pthread_main, &info)) {
        perror("pthread_create");
        exit(1);
    }

    sema_down(&info.started);
    sema_destroy(&info.started);

    // Get the same semantics as Pintos by calling detach.
    pthread_detach(created);

    return created;
}

tid_t thread_current(void) {
    return pthread_self();
}

void thread_exit(void) {
    pthread_exit(NULL);
}

void thread_yield(void) {
    sched_yield();
}

void timer_msleep(unsigned ms) {
    struct timespec time = { ms / 1000, (ms % 1000) * 1000000L };
    nanosleep(&time, NULL);
}

/**
 * Semaphore.
 */

void sema_init(struct semaphore *sema, unsigned value) {
    sem_init(&sema->os, 0, value);
}

void sema_destroy(struct semaphore *sema) {
    sem_destroy(&sema->os);
}

void sema_down(struct semaphore *sema) {
    while (sem_wait(&sema->os) != 0) {
        if (errno != EINTR) {
            perror("sem_wait");
            exit(1);
        }
    }
}

void sema_up(struct semaphore *sema) {
    sem_post(&sema->os);
}

/**
 * Lock.
 */

void lock_init(struct lock *lock) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&lock->os, &attr);
    pthread_mutexattr_destroy(&attr);
}

void lock_destroy(struct lock *lock) {
    pthread_mutex_destroy(&lock->os);
}

void lock_acquire(struct lock *lock) {
    pthread_mutex_lock(&lock->os);
}

void lock_release(struct lock *lock) {
    pthread_mutex_unlock(&lock->os);
}

/**
 * Condition.
 */

void cond_init(struct condition *cond) {
    pthread_cond_init(&cond->os, NULL);
}

void cond_destroy(struct condition *cond) {
    pthread_cond_destroy(&cond->os);
}

void cond_wait(struct condition *cond, struct lock *lock) {
    pthread_cond_wait(&cond->os, &lock->os);
}

void cond_signal(struct condition *cond, struct lock *lock) {
    UNUSED(lock);
    pthread_cond_signal(&cond->os);
}

void cond_broadcast(struct condition *cond, struct lock *lock) {
    UNUSED(lock);
    pthread_cond_broadcast(&cond->os);
}

/**
 * Timer.
 */

int64_t timer_ticks(void) {
    struct timespec time = { 0, 0 };
    clock_gettime(CLOCK_MONOTONIC, &time);

    return 1000000000*(int64_t)time.tv_sec + time.tv_nsec;
}

double timer_elapsed(int64_t since) {
    int64_t now = timer_ticks();

    return (now - since) / 1000000.0;
}

#endif


#ifdef WIN32

static DWORD WINAPI win32_start(void *data) {
    thread_main(data);
    return 0;
}

tid_t thread_create(const char *name, int priority, thread_func *fn, void *aux) {
    DWORD thread_id;
    HANDLE created;
    struct thread_start info;
    info.run = fn;
    info.aux = aux;
    sema_init(&info.started, 0);

    UNUSED(name);
    UNUSED(priority);

    created = CreateThread(NULL, 0, &win32_start, &info, 0, &thread_id);
    if (!created) {
        printf("Failed creating thread %d\n", GetLastError());
        exit(1);
    }

    CloseHandle(created);

    sema_down(&info.started);
    sema_destroy(&info.started);

    return thread_id;
}

tid_t thread_current(void) {
    return GetCurrentThreadId();
}

void thread_exit(void) {
    ExitThread(0);
}

void thread_yield(void) {
    Sleep(0);
}

void timer_msleep(unsigned ms) {
    Sleep(ms);
}

/**
 * Semaphore.
 */

void sema_init(struct semaphore *sema, unsigned value) {
    // Note: We specify a value that should be large enough for 'maxCount'.
    sema->os = CreateSemaphore(NULL, value, 10000, NULL);
}

void sema_destroy(struct semaphore *sema) {
    CloseHandle(sema->os);
}

void sema_down(struct semaphore *sema) {
    WaitForSingleObject(sema->os, INFINITE);
}

void sema_up(struct semaphore *sema) {
    ReleaseSemaphore(sema->os, 1, NULL);
}

/**
 * Lock.
 */

void lock_init(struct lock *lock) {
    InitializeCriticalSection(&lock->os);
}

void lock_destroy(struct lock *lock) {
    DeleteCriticalSection(&lock->os);
}

void lock_acquire(struct lock *lock) {
    EnterCriticalSection(&lock->os);
}

void lock_release(struct lock *lock) {
    LeaveCriticalSection(&lock->os);
}

/**
 * Condition.
 */

void cond_init(struct condition *cond) {
    InitializeConditionVariable(&cond->os);
}

void cond_destroy(struct condition *cond) {
    // Nothing to do...
    UNUSED(cond);
}

void cond_wait(struct condition *cond, struct lock *lock) {
    SleepConditionVariableCS(&cond->os, &lock->os, INFINITE);
}

void cond_signal(struct condition *cond, struct lock *lock) {
    UNUSED(lock);
    WakeConditionVariable(&cond->os);
}

void cond_broadcast(struct condition *cond, struct lock *lock) {
    UNUSED(lock);
    WakeAllConditionVariable(&cond->os);
}

/**
 * Timer.
 */

int64_t timer_ticks(void) {
    LARGE_INTEGER out;
    QueryPerformanceCounter(&out);
    return out.QuadPart;
}

double timer_elapsed(int64_t since) {
    int64_t now = timer_ticks();
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    return (now - since) / (freq.QuadPart / 1000.0);
}

#endif
