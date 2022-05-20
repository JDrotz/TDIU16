#pragma once
#include "os.h"

typedef void thread_func(void *aux);
tid_t thread_create(const char *name, int priority, thread_func *fn, void *aux);

tid_t thread_current(void);
void thread_exit(void);
void thread_yield(void);

void timer_msleep(unsigned milliseconds);

// Some macro magic to avoid type errors with "thread_new".
#define thread_new(fn, aux) thread_create("", 0, (thread_func *)(fn), aux)

// Special syntax in Progvis.
#define NO_STEP
