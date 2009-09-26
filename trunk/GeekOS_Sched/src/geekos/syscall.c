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
			 g_currentThread->userContext->semaphores = (int
			 *)Malloc((userSemaCnt+1)*sizeof(int));
			 
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
/*### Funcion que chequea si el kthread actual tiene en su lista de semaforos
* el sem con id SID.
*	REQUIRES:
*		SID € [0, MAX_NUM_SEMAPHORES-1 ]
*	RETURNS:
*		true <==> valid SID & Sema[SID].active == TRUE
* 		false otherwise
*###*/
static bool its_allowed (int SID)
{
	bool result = false;
	int listSize = 0, i = 0;
	int * semList = NULL;
	
	/* si no esta dentro del rango valido devolvemos false */
	if (SID < 0 || SID >= MAX_NUM_SEMAPHORES)
		return false;
	
	/* si esta dentro del rango, pero... esta activado? */
	if (Sema[SID].active == false)
		return false;
	
	/* ahora obtenemos los valores del user_context */
	listSize = g_currentThread->userContext->activeSemaCnt;
	semList = g_currentThread->userContext->semaphores;
	
	/* lo buscamos en la lista */
	for (i = 0; i < listSize; i++) {
		if (semList[i] == SID) {
			/* lo encontramos */
			result = true;
			break;
		}
	}
	
	return result;
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

/*### Que deberiamos hacer para destruir el semaforo:
* Primero ver que esta intentando eliminar un semaforo correcto: (its_allowed).
* Si esta habilitado para borrar chequear la cantidad de referencias de
diferentes
* threads que tienen el semaforo, si es mayor que uno NO se elimina el semaforo
* ya que hay mas threads usandolo.
* En cualquier caso debemos eliminarlo de la lista de semaforos del kthread 
* (del User_Context->semaphores[]).
* En caso de que sea el unico thread usando el semaforo => desactivamos el 
* semaforo de Sem[SID].active = FALSE.
*###*/
/*
* Destroy a semaphore.
* Params:
*   state->ebx - the semaphore id
*
* Returns: 0 if successful, error code (< 0) if unsuccessful
*/
static int Sys_DestroySemaphore(struct Interrupt_State* state)
{
	/** verificar el tema de Atomic y las interrupciones... */
	bool atom = Begin_Int_Atomic();	
	int SID = state->ebx;
	
	/* verificamos si esta habilitado, si no devolvemos -1 */
	if (!its_allowed(SID))
		return -1;
	
	/* Ahora, como en cualquier caso tenemos que eliminarlo de nuestra lista
	* de semaforos (osea del User_context) lo eliminamos */
	del_sem_from_list (SID); /** NOTE: fucking FIXME :) (falta implementar).
	* yo diria que usemos arreglos de tamaño constante (i.e: userContext->
	* semaphores[MAX_NUM_SEMAPHORES] para evitarnos bastantes kilombos, 
	* seteando en numeros negativos los semaforos deshabilitados del arreglo
	* y listo :)
	*/
	/* ahora vemos la cantidad de threads que estan referenciando este
	* semaforo */
	if (Sema[SID].threads_using > 1) {
		/* simplemente decrementamos en uno la cantidad de threads que
		* referencian el sem */
		Sema[SID].threads_using--;
	} else {
		/* tenemos que eliminarlo a la mierda porque es = 1 (osea es el
		* ultimo thread que lo referencia */
		Sema[SID].threads_using = 0;
		Sema[SID].count = 0;	/* al pedo pero bueno, limpiamos :) */
		Sema[SID].active = false;	/* con esto alcanzaria */
		/* limpiamos el nombre */
		memset (Sema[SID].name, '\0', MAX_NAME_LEN);
	}
	
	
	End_Int_Atomic(atom);
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
