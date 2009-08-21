/*
 * Common user mode functions
 * Copyright (c) 2001,2003,2004 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.50 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/int.h>
#include <geekos/mem.h>
#include <geekos/malloc.h>
#include <geekos/kthread.h>
#include <geekos/vfs.h>
#include <geekos/tss.h>
#include <geekos/user.h>

/*
 * This module contains common functions for implementation of user
 * mode processes.
 */

/*
 * Associate the given user context with a kernel thread.
 * This makes the thread a user process.
 */
void Attach_User_Context(struct Kernel_Thread* kthread, struct User_Context* context)
{
    KASSERT(context != 0);
    kthread->userContext = context;

    /*Disable_Interrupts();*/
    {bool iflag = Begin_Int_Atomic();
    /*
     * We don't actually allow multiple threads
     * to share a user context (yet)
     */
    KASSERT(context->refCount == 0);

    ++context->refCount;
    End_Int_Atomic(iflag);}
    /*Enable_Interrupts();*/
}

/*
 * If the given thread has a user context, detach it
 * and destroy it.  This is called when a thread is
 * being destroyed.
 */
void Detach_User_Context(struct Kernel_Thread* kthread)
{
    struct User_Context* old = kthread->userContext;

    kthread->userContext = 0;

    if (old != 0) {
	int refCount;

    KASSERT(Interrupts_Enabled());

	Disable_Interrupts();
        --old->refCount;
	refCount = old->refCount;
	Enable_Interrupts();

	/*Print("User context refcount == %d\n", refCount);*/
        if (refCount == 0)
            Destroy_User_Context(old);
    }
}

/*
 * Spawn a user process.
 * Params:
 *   program - the full path of the program executable file
 *   command - the command, including name of program and arguments
 *   pThread - reference to Kernel_Thread pointer where a pointer to
 *     the newly created user mode thread (process) should be
 *     stored
 * Returns:
 *   The process id (pid) of the new process, or an error code
 *   if the process couldn't be created.  Note that this function
 *   should return ENOTFOUND if the reason for failure is that
 *   the executable file doesn't exist.
 */
int Spawn(const char *program, const char *command, struct Kernel_Thread **pThread)
{
    int rc; /* The return code for the various procedure's calls*/
    void *buf;
    ulong_t fileLen;
    bool detached = false;
    struct Exe_Format exeFormat;
    struct User_Context *pUserContext;


    /* Print("Spawning %s...\n", program); */
    rc = Read_Fully(program,(void *)&buf, &fileLen);
    if (rc != 0){/* Something went wrong */
        /*Print("Spawn: Couldn't read the executable. File %s doesn't exist!\n", program);*/
        return rc;
    }else{
        /*Print("Spawn: the executable %s exist!\n", program);*/
    }
    rc = Parse_ELF_Executable((char *)buf, fileLen, &exeFormat);
    if(rc != 0){
        /*Print("Couldn't parse ELF executable!\n");*/
        Free(buf);
        return rc;
    }
    rc = Load_User_Program((char *)buf, fileLen, &exeFormat, command, &pUserContext);
    if(rc != 0){
        Print("Spawn: Ouch! Could not load user program!\n");
        Free(buf);
        return rc;
    }
    (*pThread) = Start_User_Thread(pUserContext, detached);
    if ((*pThread) == 0){
        /*Print("Spawn: Aaaargh! Getting a null kernel thread!\n");*/
        Free(buf);
        return -1;
    }
    Free(buf);
    return (*(*pThread)).pid;
    
    TODO("Spawn a process by reading an executable from a filesystem");
}

/*
 * If the given thread has a User_Context,
 * switch to its memory space.
 *
 * Params:
 *   kthread - the thread that is about to execute
 *   state - saved processor registers describing the state when
 *      the thread was interrupted
 */
void Switch_To_User_Context(struct Kernel_Thread* kthread, struct Interrupt_State* state)
{
    /* First of all check that the given thread has a User_Context attached to it */
    if(((*kthread).userContext == 0) || !Is_User_Interrupt(state) )/* Nothing to do */
        return;

    /* En ocasiones '(state == (*kthread).esp)', pero en otras no,
     * por lo que debemos setear como stack del kernel al interrupt state.
     * Aparentemente no son iguales cuando se produce un salto en la rutina 'Handle interrupt'.
     *
     * Supongo que el seteo del stack pointer del kernel es necesario porque al retornar de
     * una interrupcion, el stack que se usa para llenar los registros es la que figura
     * en el TSS.
     */
    Set_Kernel_Stack_Pointer((ulong_t)(*kthread).stackPage + (ulong_t)PAGE_SIZE);
    Switch_To_Address_Space((*kthread).userContext);
    return;
    TODO("Switch to a new user address space, if necessary");
}

