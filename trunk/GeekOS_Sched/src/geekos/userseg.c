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

/* TODO: Implement
static struct User_Context* Create_User_Context(ulong_t size)
*/


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
    Free(userContext->memory);
    Free_Segment_Descriptor(userContext->ldtDescriptor);
    Free(userContext);
    return;
    TODO("Destroy a User_Context");
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
    unsigned numArgs;
    ulong_t argBlockSize,
        segmentsSize,
        segmentSize;      /* The size of the user segment */

     /* Alloc room for a new user context */
     /* This memory will be freed when the user thread be destroyed */
    (*pUserContext) = (struct User_Context *)Malloc(sizeof(struct User_Context ));
    if ((*pUserContext)==0){
        Print("Load_User_Program: Damn! I could not alloc space for user context!\n");
        Print("Load_User_Program: Are you freeing memory?\n");
        return -1;
    }

    (*(*pUserContext)).refCount=0;

    /* Determine where in memory each executable segment will be placed */
        /* Before the arg block and the stack. Done!*/

    /* Determine size of argument block and where it memory it will be placed*/
    Get_Argument_Block_Size(command, &numArgs, &argBlockSize);
        /* The argument block will be placed before the stack. Done! */

    {/* Determine size of the segments block */
    int i = 0;
    segmentsSize = 0;
    while (i<(*exeFormat).numSegments){
        segmentsSize =
            (
                segmentsSize <
                (((*exeFormat).segmentList[i]).startAddress) + (((*exeFormat).segmentList[i]).sizeInMemory)
            )? (((*exeFormat).segmentList[i]).startAddress) + (((*exeFormat).segmentList[i]).sizeInMemory)
            :segmentsSize;
        ;
        i++;
    }}
    /* (?) Hay que alojar memoria para el proceso. A partir de esto se puede 'traducir' las direcciones que aparecen en los 'exe segments' -por ejemplo entry address-.*/
    segmentSize = Round_Up_To_Page(segmentsSize + argBlockSize + DEFAULT_USER_STACK_SIZE);
    
    /** De aca en adelante SE CAMBIA MALLOC PARA PAGING */
    (*(*pUserContext)).memory =(char *)Malloc(segmentSize);
    if ((*(*pUserContext)).memory == 0){
        Print("Load_User_Program: Oh no! I could not alloc memory for user process!\n");
        Free(*pUserContext);
        return -1;
    }
    (*(*pUserContext)).size = segmentSize;

    memset((*(*pUserContext)).memory, '\0', (*(*pUserContext)).size);

    /* Copy each executable segment into memory */
    int i = 0;
    while(i<(*exeFormat).numSegments){
        /* "There's no harm in being paranoic", that's what Dave told us! */
        KASSERT(
            (((*exeFormat).segmentList[i]).startAddress)<=segmentsSize
        );

        /* (?) Deberia usarse memcpy? */
        memcpy(/* Destination. NOTE: In our case it's given by the 'startAddress' field. */
            ((*(*pUserContext)).memory+((*exeFormat).segmentList[i]).startAddress),
            (exeFileData+(*exeFormat).segmentList[i].offsetInFile),/* Source */
            ((*exeFormat).segmentList[i].lengthInFile)                           /* Size */
         );
        i++;
    }
    /* Format argument block in memory */
    {char * bufferArgBlock = Malloc(argBlockSize+1); /* One plus for the \0 */
    Format_Argument_Block(
        bufferArgBlock, /* (?) No estoy seguro que colocar aca */
        numArgs,
        /* /?\ Ojo! Aqui va la direccion de ususrio, donde el 0 es memory!*/
        (ulong_t)(segmentsSize),
        command
    );
    /* Copiar a espacio de usuario el 'bufferArgBlock' */
    memcpy((void *)((*(*pUserContext)).memory + segmentsSize),
        bufferArgBlock,
        argBlockSize
    );

    Free(bufferArgBlock);
    }
    /* In the created User_Context object, set code entry point
     *   address, argument block address, and initial KERNEL(?) stack pointer
     *   address
     */

    KASSERT(
        ((*(*pUserContext)).memory + DEFAULT_USER_STACK_SIZE + argBlockSize + (*exeFormat).entryAddr) <=
        ((*(*pUserContext)).memory + segmentSize)
    );

    /* (?)En entry address deberia estar dezplazado?
     * No, puesto que el code selector (asi como tambien el data selector) sirven
     * para estos fines. Asi, el desplazamiento deberia especificarse en los selectores,
     * mas precisamente cuando se inicializan.
     */
    (*(*pUserContext)).entryAddr = ((*exeFormat).entryAddr);
    (*(*pUserContext)).argBlockAddr = (ulong_t)segmentsSize;
    (*(*pUserContext)).stackPointerAddr = (ulong_t)((*(*pUserContext)).size);

    /* Using Segmentation: We need to provide a private address space for the process.
     *  The following steps are needed:
     */
        /* Create an LDT descriptor in the GDT. */
    (*(*pUserContext)).ldtDescriptor = Allocate_Segment_Descriptor();
    if((*(*pUserContext)).ldtDescriptor==0){
        Print("Load_User_Program: Oh no! I could not alloc segment descriptor!\n");
        Free((*(*pUserContext)).memory);
        Free((*pUserContext));
        return -1;
    }

        /* What Dave don't tell us (maybe because it's too obvious): We need to initialize the LDT */
    Init_LDT_Descriptor(
        (*(*pUserContext)).ldtDescriptor,
        (*(*pUserContext)).ldt,
        NUM_USER_LDT_ENTRIES
    );

        /* Create an LDT selector (an index in the GDT actually)*/
    (*(*pUserContext)).ldtSelector =
        Selector(
            KERNEL_PRIVILEGE,
            true, /* The referenced segment descriptor is in the GDT */
            Get_Descriptor_Index((*(*pUserContext)).ldtDescriptor)
        );
        /* Create a code segment descriptor */
    {ulong_t numPages = (segmentSize/PAGE_SIZE);

    Init_Code_Segment_Descriptor(
        &((*(*pUserContext)).ldt[0]), /* The segment descriptor */
        (ulong_t)((*(*pUserContext)).memory),/* Base address */
        numPages,                     /* Number of pages */
        USER_PRIVILEGE                /* Privilege level */
    );
        /* Create a data segment descriptor */
    Init_Data_Segment_Descriptor(
        &((*(*pUserContext)).ldt[1]), /* The segment descriptor */
        (ulong_t)((*(*pUserContext)).memory),/* Base address */
        numPages,                     /* Number of pages */
        USER_PRIVILEGE                /* Privilege level */
    );
    }/* End of numPages scope */

        /* Create a data and code segment selectors (NOTE:Now in the LDT)*/
    (*(*pUserContext)).csSelector =
        Selector(USER_PRIVILEGE,
            false, /* The referenced segment descriptor IS NOT in the GDT */
            0
        );

    (*(*pUserContext)).dsSelector =
        Selector(USER_PRIVILEGE,
            false, /* The referenced segment descriptor IS NOT in the GDT */
            1
        );
    return 0;
    TODO("Load a user executable into a user memory space using segmentation");
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
    KASSERT(!Interrupts_Enabled());
    if(!Validate_User_Memory((*g_currentThread).userContext, srcInUser, bufSize))
    /* The user buffer don't lie entirely in memory belonging to the process */
        return false;
    memcpy(
        destInKernel,
        (srcInUser+(*(*g_currentThread).userContext).memory),
        bufSize);

    return true;
    TODO("Copy memory from user buffer to kernel buffer");
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
    KASSERT(!Interrupts_Enabled());
    if(!Validate_User_Memory((*g_currentThread).userContext, destInUser, bufSize))
    /* The user buffer don't lie entirely in memory belonging to the process */
        return false;
    memcpy(
        (destInUser + (*(*g_currentThread).userContext).memory),
        srcInKernel,
        bufSize);

    return true;
    TODO("Copy memory from kernel buffer to user buffer");
}

/*
 * Switch to user address space belonging to given
 * User_Context object.
 * Params:
 * userContext - the User_Context
 */
void Switch_To_Address_Space(struct User_Context *userContext)
{
    extern void Load_LDTR( ushort_t ltdSelector );
    /*
     * Hint: you will need to use the lldt assembly language instruction
     * to load the process's LDT by specifying its LDT selector.
     */
    KASSERT((*userContext).ldtSelector != 0);

    Load_LDTR((*userContext).ldtSelector);

    return;
    TODO("Switch to user address space using segmentation/LDT");
}

