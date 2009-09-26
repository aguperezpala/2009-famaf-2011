/*
 * Synchronization primitives
 * Copyright (c) 2001,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.13 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/kthread.h>
#include <geekos/int.h>
#include <geekos/kassert.h>
#include <geekos/screen.h>
#include <geekos/synch.h>

/*
 * NOTES:
 * - The GeekOS mutex and condition variable APIs are based on those
 *   in pthreads.
 * - Unlike disabling interrupts, mutexes offer NO protection against
 *   concurrent execution of interrupt handlers.  Mutexes and
 *   condition variables should only be used from kernel threads,
 *   with interrupts enabled.
 */


/** Private variables */
/* Invariant: s_semaphoresSize holds the number of created semaphores in the
system */
static unsigned int s_semaphoresSize=0;
static struct Semaphore * s_semaphores[MAX_SYS_SEMAPHORES];
/** End Private variables */

/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */

/*
 * The mutex is currently locked.
 * Atomically reenable preemption and wait in the
 * mutex's wait queue.
 */
static void Mutex_Wait(struct Mutex *mutex)
{
    KASSERT(mutex->state == MUTEX_LOCKED);
    KASSERT(g_preemptionDisabled);

    Disable_Interrupts();
    g_preemptionDisabled = false;
    Wait(&mutex->waitQueue);
    g_preemptionDisabled = true;
    Enable_Interrupts();
}

/*
 * Lock given mutex.
 * Preemption must be disabled.
 */
static __inline__ void Mutex_Lock_Imp(struct Mutex* mutex)
{
    KASSERT(g_preemptionDisabled);

    /* Make sure we're not already holding the mutex */
    KASSERT(!IS_HELD(mutex));

    /* Wait until the mutex is in an unlocked state */
    while (mutex->state == MUTEX_LOCKED) {
	Mutex_Wait(mutex);
    }

    /* Now it's ours! */
    mutex->state = MUTEX_LOCKED;
    mutex->owner = g_currentThread;
}

/*
 * Unlock given mutex.
 * Preemption must be disabled.
 */
static __inline__ void Mutex_Unlock_Imp(struct Mutex* mutex)
{
    KASSERT(g_preemptionDisabled);

    /* Make sure mutex was actually acquired by this thread. */
    KASSERT(IS_HELD(mutex));

    /* Unlock the mutex. */
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;

    /*
     * If there are threads waiting to acquire the mutex,
     * wake one of them up.  Note that it is legal to inspect
     * the queue with interrupts enabled because preemption
     * is disabled, and therefore we know that no thread can
     * concurrently add itself to the queue.
     */
    if (!Is_Thread_Queue_Empty(&mutex->waitQueue)) {
	Disable_Interrupts();
	Wake_Up_One(&mutex->waitQueue);
	Enable_Interrupts();
    }
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Initialize given mutex.
 */
void Mutex_Init(struct Mutex* mutex)
{
    mutex->state = MUTEX_UNLOCKED;
    mutex->owner = 0;
    Clear_Thread_Queue(&mutex->waitQueue);
}

/*
 * Lock given mutex.
 */
void Mutex_Lock(struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    g_preemptionDisabled = true;
    Mutex_Lock_Imp(mutex);
    g_preemptionDisabled = false;
}

/*
 * Unlock given mutex.
 */
void Mutex_Unlock(struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    g_preemptionDisabled = true;
    Mutex_Unlock_Imp(mutex);
    g_preemptionDisabled = false;
}

/*
 * Initialize given condition.
 */
void Cond_Init(struct Condition* cond)
{
    Clear_Thread_Queue(&cond->waitQueue);
}

/*
 * Wait on given condition (protected by given mutex).
 */
void Cond_Wait(struct Condition* cond, struct Mutex* mutex)
{
    KASSERT(Interrupts_Enabled());

    /* Ensure mutex is held. */
    KASSERT(IS_HELD(mutex));

    /* Turn off scheduling. */
    g_preemptionDisabled = true;

    /*
     * Release the mutex, but leave preemption disabled.
     * No other threads will be able to run before this thread
     * is able to wait.  Therefore, this thread will not
     * miss the eventual notification on the condition.
     */
    Mutex_Unlock_Imp(mutex);

    /*
     * Atomically reenable preemption and wait in the condition wait queue.
     * Other threads can run while this thread is waiting,
     * and eventually one of them will call Cond_Signal() or Cond_Broadcast()
     * to wake up this thread.
     * On wakeup, disable preemption again.
     */
    Disable_Interrupts();
    g_preemptionDisabled = false;
    Wait(&cond->waitQueue);
    g_preemptionDisabled = true;
    Enable_Interrupts();

    /* Reacquire the mutex. */
    Mutex_Lock_Imp(mutex);

    /* Turn scheduling back on. */
    g_preemptionDisabled = false;
}

/*
 * Wake up one thread waiting on the given condition.
 * The mutex guarding the condition should be held!
 */
void Cond_Signal(struct Condition* cond)
{
    KASSERT(Interrupts_Enabled());
    Disable_Interrupts();  /* prevent scheduling */
    Wake_Up_One(&cond->waitQueue);
    Enable_Interrupts();  /* resume scheduling */
}

/*
 * Wake up all threads waiting on the given condition.
 * The mutex guarding the condition should be held!
 */
void Cond_Broadcast(struct Condition* cond)
{
    KASSERT(Interrupts_Enabled());
    Disable_Interrupts();  /* prevent scheduling */
    Wake_Up(&cond->waitQueue);
    Enable_Interrupts();  /* resume scheduling */
}

/*
* Creates a new semaphore if 'semaName' does not exist in the list of system
semaphores,
* sets its initial count to 'count' and return the new semaphore's id.
* If there is already a semaphore named 'semaName' returns its id.
* If there is not any semaphores left returns < 0
*/
int Create_Semaphore(char * semaName, unsigned int count){
	/* Search for a semaphore named 'semaName' */
	unsigned int i = 0;
	bool found = false;
	{bool iflag = Begin_Int_Atomic();
	while(!found && i < s_semaphoresSize){
		found = strncmp((*s_semaphores[i]).name, semaName,
MAX_SEM_NAME_LENGTH)		== 0;
		i++;
	}
	End_Int_Atomic(iflag);}
	if (found) {
		/* Print("Create_Semaphore: A Semaphore named %s has been
found\n",		semaName);*/
		/* Now the semaphore is referenced by one more process */
		/* The refCount invariant would be broken if the same process
creates a		semaphore twice!*/
		{bool iflag = Begin_Int_Atomic();
		((*s_semaphores[i-1]).refCount)++;
		End_Int_Atomic(iflag);}
		/* The id of the semaphore is given by its index */
		return i-1;
	} else {
		int result=-1;
		
		/* Print("Create_Semaphore: A Semaphore named %s haven't been
found\n",		semaName);*/
		
		/* Check if there's enough room */
		{bool iflag = Begin_Int_Atomic();
		if (s_semaphoresSize==MAX_SYS_SEMAPHORES){
			End_Int_Atomic(iflag);
			return -1;
		}
		/* Good! We can proceed and create the semaphore */
		if (s_semaphores[s_semaphoresSize] == 0){ /* We only create a
semaphore			once */
			
			/* Print("Allocating room for a semaphore\n");*/
			
			s_semaphores[s_semaphoresSize]=Malloc(sizeof(struct
Semaphore));
		}
		(*s_semaphores[s_semaphoresSize]).count = count;
		(*s_semaphores[s_semaphoresSize]).refCount = 1;
		Clear_Thread_Queue(&(s_semaphores[s_semaphoresSize]->queue));
		Mutex_Init(&(s_semaphores[s_semaphoresSize]->mutex));
		/* The pointer 'semaName' may be freed, so we use strncpy */
		strcpy((*s_semaphores[s_semaphoresSize]).name, semaName);
		result = s_semaphoresSize;
		s_semaphoresSize++;
		End_Int_Atomic(iflag);}
		return result;/* The last semaphore */
	}
}

/*
* Destroys the semaphore with id 'id' if there is not process wich references
it.
* Returns: 0 if successful, < 0 ohterwise.
* Pre: 0<= id < MAX_SYS_SEMAPHORES
*/
int Destroy_Semaphore(unsigned int id){
	KASSERT(0<=id && id < MAX_SYS_SEMAPHORES);
	{bool iflag = Begin_Int_Atomic();  /* Critical section */
	
	/* Print("Destroying semaphore %s\n", s_semaphores[id]->name); */
	
	(*(s_semaphores[id])).refCount--;
	if((*(s_semaphores[id])).refCount == 0){
		/*Print("Semaphore %s has been destroyed\n",
s_semaphores[id]->name);*/
		s_semaphoresSize--;
	}
	End_Int_Atomic(iflag);}            /* End Critical section*/
	return 0;
}

int P(int id){
	KASSERT(Interrupts_Enabled());
	if(id<0 || s_semaphoresSize <= id)
		return -1;
	/* Enable_Interrupts(); */
	Mutex_Lock(&(s_semaphores[id]->mutex));
	
	/*Print("P: Called with count %d\n", s_semaphores[id]->count);*/
	
	while(s_semaphores[id]->count==0){
		bool iflag;
		Mutex_Unlock(&(s_semaphores[id]->mutex));
		iflag = Begin_Int_Atomic();
		Wait(&(s_semaphores[id]->queue));
		End_Int_Atomic(iflag);
		/* Now the interrupts are enabled, just as required by
Mutex_Lock */
		Mutex_Lock(&(s_semaphores[id]->mutex));
	}
	s_semaphores[id]->count--;
	KASSERT(0<=s_semaphores[id]->count);
	Mutex_Unlock(&(s_semaphores[id]->mutex));
	return 0;
}

int V(int id){
	KASSERT(Interrupts_Enabled());
	if(id<0 || s_semaphoresSize <= id)
		return -1;
	/* Mutex lock will enable interrups!*/
	Mutex_Lock(&(s_semaphores[id]->mutex));
    s_semaphores[id]->count++;
    Mutex_Unlock(&(s_semaphores[id]->mutex));
    Disable_Interrupts();
    Wake_Up_One(&(s_semaphores[id]->queue));
    return 0;
}

