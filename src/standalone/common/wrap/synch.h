#pragma once

#include "os.h"

struct semaphore {
    // Privat data. Använd funktionerna nedan för att manipulera semaforen.
    os_sema_t os;
};

void sema_init(struct semaphore *sema, unsigned value);
void sema_destroy(struct semaphore *sema);
void sema_down(struct semaphore *sema);
void sema_up(struct semaphore *sema);

struct lock {
    // Privat data. Använd funktionerna nedan för att manipulera låset.
    os_lock_t os;
};

void lock_init(struct lock *lock);
void lock_destroy(struct lock *lock);
void lock_acquire(struct lock *lock);
void lock_release(struct lock *lock);

struct condition {
    // Privat data. Använd funktionerna nedan för att manipulera
    // condition-variabeln.
    os_cond_t os;
};

void cond_init(struct condition *cond);
void cond_destroy(struct condition *cond);
void cond_wait(struct condition *cond, struct lock *lock);
void cond_signal(struct condition *cond, struct lock *lock);
void cond_broadcast(struct condition *cond, struct lock *lock);
