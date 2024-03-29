/*
 * Synchronization primitives
 * Copyright (c) 2001, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#ifndef GEEKOS_SYNCH_H
#define GEEKOS_SYNCH_H

#include <geekos/kthread.h>

/** Macros */
#define MAX_SEM_NAME_LENGTH 25

#define MAX_SYS_SEMAPHORES 20
/** End Macros */
/*
 * mutex states
 */
enum { MUTEX_UNLOCKED, MUTEX_LOCKED };

struct Mutex {
    int state;
    struct Kernel_Thread* owner;
    struct Thread_Queue waitQueue;
};

/*### Constantes y variables globales para los semáforos de kernel ###*/

struct Semaphore {
	char	name[MAX_SEM_NAME_LENGTH];
	uint_t	count;
	bool	active;
	uint_t	threads_using;
	struct Mutex mutex;	/* va a tener un mutex */
};

static struct Semaphore Sema[MAX_SYS_SEMAPHORES];


/*### FIN costantes de semáforos ###*/


#define MUTEX_INITIALIZER { MUTEX_UNLOCKED, 0, THREAD_QUEUE_INITIALIZER }

struct Condition {
    struct Thread_Queue waitQueue;
};

void Mutex_Init(struct Mutex* mutex);
void Mutex_Lock(struct Mutex* mutex);
void Mutex_Unlock(struct Mutex* mutex);

void Cond_Init(struct Condition* cond);
void Cond_Wait(struct Condition* cond, struct Mutex* mutex);
void Cond_Signal(struct Condition* cond);
void Cond_Broadcast(struct Condition* cond);

#define IS_HELD(mutex) \
    ((mutex)->state == MUTEX_LOCKED && (mutex)->owner == g_currentThread)

#endif  /* GEEKOS_SYNCH_H */
