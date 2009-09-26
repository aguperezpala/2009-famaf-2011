/*
 * System call handlers
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003,2004 David Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.59 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/syscall.h>
#include <geekos/errno.h>
#include <geekos/kthread.h>
#include <geekos/int.h>
#include <geekos/elf.h>
#include <geekos/malloc.h>
#include <geekos/screen.h>
#include <geekos/keyboard.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/timer.h>
#include <geekos/vfs.h>

/*
 * Null system call.
 * Does nothing except immediately return control back
 * to the interrupted user program.
 * Params:
 *  state - processor registers from user mode
 *
 * Returns:
 *   always returns the value 0 (zero)
 */
static int Sys_Null(struct Interrupt_State* state)
{
    Print("Se llamo a la Sys_Null\n");
    return 0;
}

/*
 * Exit system call.
 * The interrupted user process is terminated.
 * Params:
 *   state->ebx - process exit code
 * Returns:
 *   Never returns to user mode!
 */
static int Sys_Exit(struct Interrupt_State* state)
{
    /*TODO("Exit system call");*/
    int err = state->ebx;
    Exit(err);
    return -1;
}

/*
 * Print a string to the console.
 * Params:
 *   state->ebx - user pointer of string to be printed
 *   state->ecx - number of characters to print
 * Returns: 0 if successful, -1 if not
 */
static int Sys_PrintString(struct Interrupt_State* state)
{
    /*TODO("PrintString system call");*/
    int user = state->ebx; 
    int length = state->ecx; 
    char* buf; 

    if(length > 1024)
	return -3;
    buf = Malloc(length+1);
    if(!buf)
	return -7;
    if(!Copy_From_User(buf,user,length)){
	Free(buf);
	return -1;
    }
    buf[length]='\0';
    Print(buf);
    Free(buf);
    return 0;
}

/*
 * Get a single key press from the console.
 * Suspends the user process until a key press is available.
 * Params:
 *   state - processor registers from user mode
 * Returns: the key code
 */
static int Sys_GetKey(struct Interrupt_State* state)
{
    /*TODO("GetKey system call");*/
    Keycode key = Wait_For_Key();

    return key;
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State* state)
{
/*    TODO("SetAttr system call");*/
    Set_Current_Attr(state->ebx);
    return 0;
}

/*
 * Get the current cursor position.
 * Params:
 *   state->ebx - pointer to user int where row value should be stored
 *   state->ecx - pointer to user int where column value should be stored
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_GetCursor(struct Interrupt_State* state)
{
    int row,col;
    Get_Cursor(&row,&col);
    
    if(!Copy_To_User(state->ebx,&row,sizeof(int)))
	return -1;
    if(!Copy_To_User(state->ecx,&col,sizeof(int)))
	return -1;
	
    return 0;
}

/*
 * Set the current cursor position.
 * Params:
 *   state->ebx - new row value
 *   state->ecx - new column value
 * Returns: 0 if successful, -1 otherwise
 */
static int Sys_PutCursor(struct Interrupt_State* state)
{
    /*TODO("PutCursor system call");*/
    Put_Cursor(state->ebx,state->ecx);    
    return 0;
}

/*
 * Create a new user process.
 * Params:
 *   state->ebx - user address of name of executable
 *   state->ecx - length of executable name
 *   state->edx - user address of command string
 *   state->esi - length of command string
 * Returns: pid of process if successful, error code (< 0) otherwise
 */
static int Sys_Spawn(struct Interrupt_State* state)
{
    /*TODO("Spawn system call");*/
    struct Kernel_Thread *pThread;
    char *name,*command;
    int name_size,command_size,pid;
    
    name_size = state->ecx;
    command_size = state->esi;
    
    name = Malloc(name_size+1);
    if(!name)
	return -7;
    
    command = Malloc(command_size+1);
    if(!command){
	Free(name);
	return -7;
    }
    
    if(!Copy_From_User(name,state->ebx,name_size) || !Copy_From_User(command,state->edx,command_size)){
	Free(name);
	Free(command);
	return -2;
    }
    
    name[name_size] = '\0';
    command[command_size] = '\0';
    /*Esto hace falta porque Spawn llama a Make_Runnable_Atomic, que necesita
      tener las interrupciones habilitadas para poder deshabilitarlas (?).
      Es una falla en el disenio de GeekOS*/
    Enable_Interrupts();	
    pid = Spawn(name,command,&pThread);
    return pid;
}

/*
 * Wait for a process to exit.
 * Params:
 *   state->ebx - pid of process to wait for
 * Returns: the exit code of the process,
 *   or error code (< 0) on error
 */
static int Sys_Wait(struct Interrupt_State* state)
{
    /*TODO("Wait system call");*/
    struct Kernel_Thread* thread = Lookup_Thread(state->ebx); /*ver cambio en Lookup_Thread*/
    Enable_Interrupts();	/*igual que en Sys_Spawn. Join hace KASSERT(Interrupts_Enabled) ni bien entra*/
    int err = Join(thread);
    
    return err;
    
}
/*
 * Get pid (process id) of current thread.
 * Params:
 *   state - processor registers from user mode
 * Returns: the pid of the current thread
 */
static int Sys_GetPID(struct Interrupt_State* state)
{
    /*TODO("GetPID system call");*/
    int pid = Get_Current()->pid;
    return pid;

}
/*
* Set the scheduling policy.
* Params:
*   state->ebx - policy,
*   state->ecx - number of ticks in quantum
* Returns: 0 if successful, -1 otherwise
*/
static int Sys_SetSchedulingPolicy(struct Interrupt_State* state)
{
	KASSERT(!Interrupts_Enabled());
	Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar
las	interrupciones? */
	if (Set_SchedulingPolicy((*state).ebx) != 0)
		return -1;
	if (Set_Quantum((*state).ecx) != 0)
		return -1;
	return 0;
	TODO("SetSchedulingPolicy system call");
}

/*
* Get the time of day.
* Params:
*   state - processor registers from user mode
*
* Returns: value of the g_numTicks global variable
*/
static int Sys_GetTimeOfDay(struct Interrupt_State* state)
{
	ulong_t value = g_numTicks;
	KASSERT(!Interrupts_Enabled());
	Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar
las	interrupciones? */
	return value;
	TODO("GetTimeOfDay system call");
}

/*
* Create a semaphore.
* Params:
*   state->ebx - user address of name of semaphore
*   state->ecx - length of semaphore name
*   state->edx - initial semaphore count
* Returns: the global semaphore id
*
* Pre: 0<=state->ecx<MAX_SEM_NAME_LENGTH
*     and 0<=state->edx
*/
static int Sys_CreateSemaphore(struct Interrupt_State* state)
{
	struct Kernel_Thread * kthread = g_currentThread; /* We ensure
interrupts	don't bother us */
	KASSERT(!Interrupts_Enabled());
	
	int sid;
	KASSERT((0<=(*state).ecx) && ((*state).ecx < MAX_SEM_NAME_LENGTH) &&
	(0<=(*state).edx)
	);
	/* Copy the semaphore's name to a kernel buffer */
	char * buf = Malloc((*state).ecx * sizeof(char) + 1);
	if(!buf)
		return -1;
	if(!Copy_From_User((void*) buf, (*state).ebx, (*state).ecx)){
		Free(buf);
		return -1;
	}
	Enable_Interrupts();
	buf[(*state).ecx]='\0';
	/* Create the semaphore */
	sid = Create_Semaphore(buf, (*state).edx);
	if (!Add_Semaphore(kthread, sid)){
		Free(buf);
		return -1;
	}
	/* Free the buffer */
	Free(buf);
	KASSERT(0<=sid);
	return sid;
}

/*
* Acquire a semaphore.
* Assume that the process has permission to access the semaphore,
* the call will block until the semaphore count is >= 0.
* Params:
*   state->ebx - the semaphore id
*
* Returns: 0 if successful, error code (< 0) if unsuccessful
*/
static int Sys_P(struct Interrupt_State* state)
{
	struct Kernel_Thread * kthread = g_currentThread;
	KASSERT(!Interrupts_Enabled());
	Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar
las	interrupciones? */
	/* Check if the id is a valid one */
	if (!((0<=state->ebx) && (state->ebx < MAX_SYS_SEMAPHORES)
		)
		)
		return -1;
	/* Check if the process has permission to make the call */
	if (!Has_Semaphore(kthread, (*state).ebx))
		return -1;
	/* Now we can safely proceed */
	return P((*state).ebx);
}

/*
* Release a semaphore.
* Params:
*   state->ebx - the semaphore id
*
* Returns: 0 if successful, error code (< 0) if unsuccessful
*/
static int Sys_V(struct Interrupt_State* state)
{
	struct Kernel_Thread * kthread = g_currentThread;
	KASSERT(!Interrupts_Enabled());
	Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar
las	interrupciones? */
	/* Check if the process has permission to make the call */
	if (!Has_Semaphore(kthread, (*state).ebx))
		return -1;
	/* Now we can safely proceed */
	return V((*state).ebx);
	
}

/*
* Destroy a semaphore.
* Params:
*   state->ebx - the semaphore id
*
* Returns: 0 if successful, error code (< 0) if unsuccessful
*/
static int Sys_DestroySemaphore(struct Interrupt_State* state)
{
	struct Kernel_Thread * kthread = g_currentThread;
	KASSERT(!Interrupts_Enabled());
	Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar
las	interrupciones? */
	
	if (!Remove_Semaphore(kthread, (*state).ebx))
		return -1;
	Destroy_Semaphore(state->ebx);
	return 0;
	TODO("DestroySemaphore system call");
}


/*
* Global table of system call handler functions.
*/
const Syscall g_syscallTable[] = {
	Sys_Null,
	      Sys_Exit,
	      Sys_PrintString,
	      Sys_GetKey,
	      Sys_SetAttr,
	      Sys_GetCursor,
	      Sys_PutCursor,
	      Sys_Spawn,
	      Sys_Wait,
	      Sys_GetPID,
	      /* Scheduling and semaphore system calls. */
	      Sys_SetSchedulingPolicy,
	      Sys_GetTimeOfDay,
    Sys_CreateSemaphore,
    Sys_P,
    Sys_V,
    Sys_DestroySemaphore,
};


/*
 * Number of system calls implemented.
 */
const int g_numSyscalls = sizeof(g_syscallTable) / sizeof(Syscall);
