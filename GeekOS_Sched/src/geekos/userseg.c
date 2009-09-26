/*
 * Segmentation-based user mode implementation
 * Copyright (c) 2001,2003 David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.23 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/ktypes.h>
#include <geekos/kassert.h>
#include <geekos/defs.h>
#include <geekos/mem.h>
#include <geekos/string.h>
#include <geekos/malloc.h>
#include <geekos/int.h>
#include <geekos/gdt.h>
#include <geekos/segment.h>
#include <geekos/tss.h>
#include <geekos/kthread.h>
#include <geekos/argblock.h>
#include <geekos/user.h>
#include <geekos/errno.h>

/* ----------------------------------------------------------------------
 * Variables
 * ---------------------------------------------------------------------- */

#define DEFAULT_USER_STACK_SIZE 8192


/* ----------------------------------------------------------------------
 * Private functions
 * ---------------------------------------------------------------------- */


/*
 * Create a new user context of given size
 */

/* TODO: Implement*/
static struct User_Context* Create_User_Context(ulong_t size)
{
    struct User_Context * usr_ctxt = Malloc(sizeof(struct User_Context));
    if (usr_ctxt == NULL) {
	return NULL;
    }

    usr_ctxt -> memory = Malloc(size);
    usr_ctxt -> size = size;
    if (usr_ctxt -> memory == NULL) {
	Free (usr_ctxt);
	return NULL;
    }
    
    memset((char *) usr_ctxt -> memory, '\0', size);

    usr_ctxt -> ldtDescriptor = Allocate_Segment_Descriptor();
    if (usr_ctxt -> ldtDescriptor == 0) {
	Free (usr_ctxt->memory);
	Free (usr_ctxt);
	return NULL;
    }
    Init_LDT_Descriptor(usr_ctxt->ldtDescriptor, usr_ctxt->ldt, NUM_USER_LDT_ENTRIES);
    
    usr_ctxt -> ldtSelector = Selector(KERNEL_PRIVILEGE,true,Get_Descriptor_Index(usr_ctxt -> ldtDescriptor));
    
    Init_Code_Segment_Descriptor(
		 	       &usr_ctxt->ldt[0],
			       (unsigned long)usr_ctxt->memory, // base address
			       ((long)usr_ctxt->size/PAGE_SIZE)+10,	 // num pages
			       3		         // privilege level (3 == user)
			       );
  usr_ctxt -> csSelector = Selector( USER_PRIVILEGE, false, 0 );
  
  Init_Data_Segment_Descriptor(
			       &usr_ctxt->ldt[1],
			       (unsigned long)usr_ctxt->memory, // base address
			       ((long)usr_ctxt->size/PAGE_SIZE)+10,	 // num pages
			       3		         // privilege level (3 == user)
			       );
  usr_ctxt -> dsSelector = Selector( USER_PRIVILEGE, false, 1 );
  
  return usr_ctxt;
    
}



static bool Validate_User_Memory(struct User_Context* userContext,
    ulong_t userAddr, ulong_t bufSize)
{
    ulong_t avail;
    if (userAddr >= userContext->size)
        return false;

    avail = userContext->size - userAddr;
    if (bufSize > avail)
        return false;

    return true;
}

/* ----------------------------------------------------------------------
 * Public functions
 * ---------------------------------------------------------------------- */

/*
 * Destroy a User_Context object, including all memory
 * and other resources allocated within it.
 */
void Destroy_User_Context(struct User_Context* userContext)
{
    /*
     * Hints:
     * - you need to free the memory allocated for the user process
     * - don't forget to free the segment descriptor allocated
     *   for the process's LDT
     */
/*    TODO("Destroy a User_Context");*/
    Free(userContext->memory);
    Free_Segment_Descriptor(userContext->ldtDescriptor);
    Free(userContext);
}

/*
 * Load a user executable into memory by creating a User_Context
 * data structure.
 * Params:
 * exeFileData - a buffer containing the executable to load
 * exeFileLength - number of bytes in exeFileData
 * exeFormat - parsed ELF segment information describing how to
 *   load the executable's text and data segments, and the
 *   code entry point address
 * command - string containing the complete command to be executed:
 *   this should be used to create the argument block for the
 *   process
 * pUserContext - reference to the pointer where the User_Context
 *   should be stored
 *
 * Returns:
 *   0 if successful, or an error code (< 0) if unsuccessful
 */
int Load_User_Program(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat, const char *command,
    struct User_Context **pUserContext)
{
    /*
     * Hints:
     * - Determine where in memory each executable segment will be placed
     * - Determine size of argument block and where it memory it will
     *   be placed
     * - Copy each executable segment into memory
     * - Format argument block in memory
     * - In the created User_Context object, set code entry point
     *   address, argument block address, and initial kernel stack pointer
     *   address
     */
    unsigned long virtSize;
    void * virtSpace;
    int i;
    int numArgs;
    ulong_t maxva = 0, argSize;
    
    /* Find maximum virtual address */
    for (i = 0; i < exeFormat->numSegments; ++i) {
	struct Exe_Segment *segment = &exeFormat->segmentList[i];
	ulong_t topva = segment->startAddress + segment->sizeInMemory; 
    
	if (topva > maxva)
    	    maxva = topva;
    }

    /* setup some memory space for the program */
    Get_Argument_Block_Size(command, &numArgs, &argSize);
    virtSize = Round_Up_To_Page(maxva)+Round_Up_To_Page(DEFAULT_USER_STACK_SIZE + argSize);

    (*pUserContext) = Create_User_Context(virtSize);
    if (*pUserContext == NULL) {
	return ENOMEM;
    }
    
    virtSpace = (*pUserContext)->memory;

    /* Load segment data into memory */
    for (i = 0; i < exeFormat->numSegments; ++i) {
	struct Exe_Segment *segment = &exeFormat->segmentList[i];

	memcpy(virtSpace + segment->startAddress,
	   exeFileData + segment->offsetInFile,
	   segment->lengthInFile);
    }
    
    /*Format argument block into memory*/
    Format_Argument_Block(virtSpace+virtSize-argSize, numArgs, virtSize-argSize, command);
    
    (*pUserContext)->entryAddr = exeFormat->entryAddr;
    (*pUserContext)->stackPointerAddr = virtSize-argSize;
    (*pUserContext)->argBlockAddr = virtSize-argSize;
    
    return 0;
}

/*
 * Copy data from user memory into a kernel buffer.
 * Params:
 * destInKernel - address of kernel buffer
 * srcInUser - address of user buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_From_User(void* destInKernel, ulong_t srcInUser, ulong_t bufSize)
{
    /*
     * Hints:
     * - the User_Context of the current process can be found
     *   from g_currentThread->userContext
     * - the user address is an index relative to the chunk
     *   of memory you allocated for it
     * - make sure the user buffer lies entirely in memory belonging
     *   to the process
     */
    /*TODO("Copy memory from user buffer to kernel buffer");*/
    if(Validate_User_Memory(Get_Current()->userContext,srcInUser,bufSize)){ /* delete this; keeps gcc happy */
        memcpy(destInKernel,(Get_Current()->userContext)->memory+srcInUser,bufSize);    
	return true;
    }
    else
	return false;
}

/*
 * Copy data from kernel memory into a user buffer.
 * Params:
 * destInUser - address of user buffer
 * srcInKernel - address of kernel buffer
 * bufSize - number of bytes to copy
 *
 * Returns:
 *   true if successful, false if user buffer is invalid (i.e.,
 *   doesn't correspond to memory the process has a right to
 *   access)
 */
bool Copy_To_User(ulong_t destInUser, void* srcInKernel, ulong_t bufSize)
{
    /*
     * Hints: same as for Copy_From_User()
     */
    /*TODO("Copy memory from kernel buffer to user buffer");*/
    if(Validate_User_Memory(Get_Current()->userContext,destInUser,bufSize)){ /* delete this; keeps gcc happy */
        memcpy((Get_Current()->userContext)->memory+destInUser,srcInKernel,bufSize);    
	return true;
    }
    else
	return false;
}

/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */

/*Routine to load the LDTR register, defined in lowlevel.asm*/
void Load_LDTR (ulong_t selector);
 
void Switch_To_Address_Space(struct User_Context *userContext)
{
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
     Load_LDTR(userContext->ldtSelector);
     
}

