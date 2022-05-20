#pragma once

/**
 * OS specific types. Contains definitions for Win32 and POSIX systems.
 */

#if defined(_WIN64)
#define WIN32
#elif defined(_WIN32)
#define WIN32
#else
#define POSIX
#endif

#define UNUSED(x) ((void)x)

/**
 * POSIX implementation.
 */
#ifdef POSIX

// Define more things in the system headers.
#define _POSIX_C_SOURCE 20000101L

#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

typedef pthread_t tid_t;
typedef sem_t os_sema_t;
typedef pthread_mutex_t os_lock_t;
typedef pthread_cond_t os_cond_t;

#endif


/**
 * Win32 implementation.
 */
#ifdef WIN32

#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>

// Old versions do not have a stdbool.h...
enum bool { false, true };
typedef enum bool bool;

// Nor stdint.h
typedef long long int int64_t;

typedef DWORD tid_t;
typedef HANDLE os_sema_t;
typedef CRITICAL_SECTION os_lock_t;
typedef CONDITION_VARIABLE os_cond_t;

// Some other defines that we need.
#define snprintf _snprintf

#endif
