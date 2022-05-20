#pragma once
#include "os.h"

#if 0
/**
 * Atomiska operationer ekvivalenta med följande kod (under antagandet att operationerna körs atomärt):
 *
 * Notera: 'atomic_swap(int *, int *)' går inte att implementera, så den ser lite annorlunda ut här.
 */

int test_and_set(int *value) {
    int old = *value;
    *value = 1;
    return old;
}

int atomic_swap(int *value, int replace) {
    int old = *value;
    *value = replace;
    return old;
}

void *atomic_swap_ptr(void **value, void *replace) {
    void *old = *value;
    *value = replace;
    return old;
}

int compare_and_swap(int *value, int compare, int swap) {
    int old = *value;
    if (old == compare)
        *value = swap;
    return old;
}

void *compare_and_swap_ptr(void **value, void *compare, void *swap) {
    void *old = *value;
    if (old == compare)
        *value = swap;
    return old;
}

int atomic_add(int *value, int add) {
    int old = *value;
    *value += add;
    return old;
}

int atomic_sub(int *value, int add) {
    int old = *value;
    *value -= add;
    return old;
}

#endif

#ifdef POSIX

static inline int test_and_set(int volatile *value) {
    return __sync_fetch_and_or(value, 1);
}

static inline int atomic_swap(int volatile *value, int replace) {
    return __sync_lock_test_and_set(value, replace);
}

static inline void *atomic_swap_ptr(void *volatile *value, void *replace) {
    return __sync_lock_test_and_set(value, replace);
}

static inline int compare_and_swap(int volatile *value, int compare, int swap) {
    return __sync_val_compare_and_swap(value, compare, swap);
}

static inline void *compare_and_swap_ptr(void *volatile *value, void *compare, void *swap) {
    return __sync_val_compare_and_swap(value, compare, swap);
}

static inline int atomic_add(int volatile *value, int add) {
    return __sync_fetch_and_add(value, add);
}

static inline int atomic_sub(int volatile *value, int sub) {
    return __sync_fetch_and_sub(value, sub);
}

#endif


#ifdef WIN32

static int test_and_set(int volatile *value) {
    return _InterlockedOr(value, 1);
}

static int atomic_swap(int volatile *value, int replace) {
    return InterlockedExchange(value, replace);
}

static void *atomic_swap_ptr(void *volatile *value, void *replace) {
    return InterlockedExchangePointer(value, replace);
}

static int compare_and_swap(int volatile *value, int compare, int swap) {
    return InterlockedCompareExchange(value, swap, compare);
}

static void *compare_and_swap_ptr(void *volatile *value, void *compare, void *swap) {
    return InterlockedCompareExchangePointer(value, swap, compare);
}

static int atomic_add(int volatile *value, int add) {
    return InterlockedExchangeAdd(value, add);
}

static int atomic_sub(int volatile *value, int sub) {
    return InterlockedExchangeAdd(value, -sub);
}

#endif
