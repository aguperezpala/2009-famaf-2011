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
#include <geekos/synch.h>

/*### Constantes y variables globales para los semáforos de kernel ###*/

#define  MAX_NUM_SEMAPHORES  20  /* Podrían ser más... */
#define  MAX_NAME_LEN  25

struct Semaphore {
	char	name[MAX_NAME_LEN];
	uint_t	count;
	bool	active;
	uint_t	threads_using;
};

static struct Semaphore Sema[MAX_NUM_SEMAPHORES];


/*### FIN costantes de semáforos ###*/

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
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar las interrupciones? */
    Exit(state->ebx);
    KASSERT(false);/* If we got here, something's wrong!*/
    TODO("Exit system call");
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
    KASSERT(!Interrupts_Enabled());
    /* We don't enable interrupts yet because Copy_From_User need interrupts to be disabled */
    char * buf = Malloc(((*state).ecx)+1); /* One plus for the '\0' character */
    if (buf == 0)
        return -1;
    if (!Copy_From_User((void *)buf, (ulong_t)(*state).ebx, (*state).ecx)){
        Free(buf);
        return -1;
    }
    /* Now we can reenable interrupts */

    buf[(*state).ecx]='\0';
//     {bool iflag = Begin_Int_Atomic();
    Print(buf);
//     End_Int_Atomic(iflag);}
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();
    Free(buf);
    return 0;
    TODO("PrintString system call");
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
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar las interrupciones? */
    return (int) Wait_For_Key();
    TODO("GetKey system call");
}

/*
 * Set the current text attributes.
 * Params:
 *   state->ebx - character attributes to use
 * Returns: always returns 0
 */
static int Sys_SetAttr(struct Interrupt_State* state)
{
    TODO("SetAttr system call");
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
    int row, col;

    KASSERT(!Interrupts_Enabled());

    Get_Cursor(&row,&col);

    if(!Copy_To_User((ulong_t)(*state).ebx, (void*) &row, sizeof(int)))
        return -1;
    if(!Copy_To_User((ulong_t)(*state).ecx, (void*) &col, sizeof(int)))
        return -1;
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();
    return 0;
    TODO("GetCursor system call");
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
    TODO("PutCursor system call");
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
    int rc;
    char * program, * command;
    struct Kernel_Thread ** dummyThread=0;
    KASSERT(!Interrupts_Enabled());

    program = Malloc(((*state).ecx)+1);
    if(program == 0){
        return -1;
    }
    command = Malloc(((*state).esi)+1);
    if(command == 0){
        Free(program);/* 'command' here is not null */
        return -1;
    }
    if(!Copy_From_User((void *)program, (ulong_t)(*state).ebx, (*state).ecx))
        return -1;

    if(!Copy_From_User((void *)command, (ulong_t)(*state).edx, (*state).esi))
        return -1;
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();
    program[((*state).ecx)] = '\0';
    command[((*state).esi)] = '\0';

    rc = Spawn(program, command, dummyThread);

    Free(program);
    Free(command);
    /* Enable_Interrupts(); */
    return rc;
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
    int rc;
    KASSERT(!Interrupts_Enabled());
    /* Join requires interrupts to be disabled! */
    rc = Join(Lookup_Thread((*state).ebx));
    /* Enable_Interrupts(); -- Join will enable interrupts */
    return rc;
    TODO("Wait system call");
}

/*
 * Get pid (process id) of current thread.
 * Params:
 *   state - processor registers from user mode
 * Returns: the pid of the current thread
 */
static int Sys_GetPID(struct Interrupt_State* state)
{
    struct Kernel_Thread * kthread;
    kthread = g_currentThread;
    KASSERT(!Interrupts_Enabled());
    Enable_Interrupts();/* (?) Cuando es el momento adecuado para habilitar las interrupciones? */
    return (*kthread).pid;
    TODO("GetPID system call");
}

/* ###
 * Set the scheduling policy.
 * Params:
 *   state->ebx - policy,
 *   state->ecx - number of ticks in quantum
 * Returns: 0 if successful, -1 otherwise
 * ###
 */
static int Sys_SetSchedulingPolicy(struct Interrupt_State* state)
{
	/* ¿Necesitaría ser atómico? */
	return Set_Scheduling_Policy (state->ebx, state->ecx);
	
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
	return (int) Get_gNumTicks ();
    TODO("GetTimeOfDay system call");
}


/* ###
 * Busca un semáforo libre en el arreglo global de semáforos.
 * Si (name) != NULL => busca semáforos con ese nombre y devuelve el primero
 * Si (name) == NULL => busca el 1º semáforo libre
 *
 *    SID = Find_Free_Semaphore (name)
 *
 * POS: SID >= 0 => En SID está el nº buscado
 *	SID <  0 => No hay semáforos libres
 * ###
 */
static int Find_Free_Semaphore (const char *name)
{
	int i = 0, SID = 0;
	
	for (i=0 ; i<MAX_NUM_SEMAPHORES ; i++) {
		if (name == NULL && !Sema[i].active)
			SID = i;
		else if (!strncmp (name, Sema[i].name, 25))
			SID = i;
	}
	
	if (i == MAX_NUM_SEMAPHORES)
		SID = -1;
	
	return SID;
}


/* ###
 * Create a semaphore.
 * Params:
 *   state->ebx - user address of name of semaphore
 *   state->ecx - length of semaphore name
 *   state->edx - initial semaphore count
 * Returns: the global semaphore id
 * ###
 */
static int Sys_CreateSemaphore(struct Interrupt_State* state)
{
	int size = 0, SID = 0;
	int userSemaCnt = 0, *userSemaList = NULL;
	char userSemaName[MAX_NAME_LEN];
	bool valid = false;
	
	size = MIN (MAX_NAME_LEN, state->ecx);
	/** GUARDA CON ESE MIN */
	KASSERT (state->ecx != 0); /* No toleramos nombres vacíos */
	
	memset (userSemaName, '\0', MAX_NAME_LEN);
	valid = Copy_From_User (userSemaName, (ulong_t) state->ebx, size);
	
	SID = Find_Free_Semaphore (userSemaName);
	
	if (SID < 0) {
	/* No existía un semáforo con ese nombre => creamos uno */
		SID = Find_Free_Semaphore (NULL);
		
		/* Actualizamos la lista de semáforos del usuario */
		userSemaCnt = g_currentThread->userContext->activeSemaCnt;
		userSemaList = (int *) Malloc (userSemaCnt * sizeof(int));
		memcpy (userSemaList,
			g_currentThread->userContext->semaphores,
			userSemaCnt * sizeof(int));
		
		Free (g_currentThread->userContext->semaphores);
		g_currentThread->userContext->semaphores = (int *)Malloc((userSemaCnt+1)*sizeof(int));
		
		memcpy (g_currentThread->userContext->semaphores,
			userSemaList,
			userSemaCnt * sizeof (int));
		g_currentThread->userContext->semaphores[userSemaCnt] = SID;
		
		/* Seteamos valores del semáforo */
		memcpy(Sema[SID].name, userSemaName, size);
		Sema[SID].count = state->edx;
	}
	
	if (SID >= 0) {
		Sema[SID].threads_using++;
		Sema[SID].active = true;
	}
	
	return SID;
	TODO("CreateSemaphore system call");
}

/** Aca deberiamos crear una funcion "bool its_allowed (int SID)" que chequea
  * si el kernel_thread actual tiene al Semaphore ID en la lista (supongo
  * que se refiere a la lista del user_context) Y ademas chequea que este
  * creado (osea, active = true)
  */

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
	int SID = state->ebx;
	bool atom = Begin_Int_Atomic();	/** lo hacemos atomico ....? */
	
	/* Primero que todo chequeamos si tenemos "permiso" (usando la funcion
	 * NO hecha y comentada arriba) para acceder al semaforo ese, si no 
	 * devolvemos con error */
	if (!its_allowed (state->ebx))
		return -1;
	/* aca estamos porque SI estamos habilitados para tocar el semaforo, que
	 * en teoria es la posicion en el arreglo Sema[] */
	
	/* aumentamos en uno la cantidad de "threads" usando este semaforo */
	Sema[SID].threads_using++;
	
	/* Debemos verificar si bloqueamos o no al thread */
	if (Sema[SID].count < 0){	/* debemos bloquear */
		/* Wait nos pide que sea sin interrupciones => para que sea
		 * atomica la llamada, pero, que pasa? que se ejecuta una vez
		 * que se duerme este thread? sigue ejecutando la siguiente
		 * instruccion? o que saca otro thread y no sigue debajo
		 * del Wait este?....
		 */
		Wait(Wait_Queue); /* esta lista esta definida kthread.h */
		/* deshabilitamos interrupciones? sigue corriendo por aca? */
		
		/* decrementamos el semaforo */
		Sema[SID].count--;
	}
	
		
	End_Int_Atomic(atom);
	return 0;
	
    TODO("P (semaphore acquire) system call");
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
	int SID = state->ebx;
	bool atom = Begin_Int_Atomic();	/** lo hacemos atomico ....? */
	
	/* Primero que todo chequeamos si tenemos "permiso" (usando la funcion
	* NO hecha y comentada arriba) para acceder al semaforo ese, si no 
	* devolvemos con error */
	if (!its_allowed (state->ebx))
		return -1;
	/* aca estamos porque SI estamos habilitados para tocar el semaforo, que
	* en teoria es la posicion en el arreglo Sema[] */
	
	/* decrementamos en uno la cantidad de threads que usan este sem */
	Sema[SID].threads_using--;
	
	/* simplemente lo que hacemos es aumentar en uno el count y levantar
	 * algun thread dando en la Wait_Queue */
	Sema[SID].count++;
	/** Requiere interrupciones deshabilitadas :(... verificar esto */
	Wake_Up_One(Wait_Queue);
	
	End_Int_Atomic(atom);
	return 0;
    TODO("V (semaphore release) system call");
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
