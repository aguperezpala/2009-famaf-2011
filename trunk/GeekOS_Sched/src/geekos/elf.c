/*
 * ELF executable loading
 * Copyright (c) 2003, Jeffrey K. Hollingsworth <hollings@cs.umd.edu>
 * Copyright (c) 2003, David H. Hovemeyer <daveho@cs.umd.edu>
 * $Revision: 1.29 $
 * 
 * This is free software.  You are permitted to use,
 * redistribute, and modify it as specified in the file "COPYING".
 */

#include <geekos/errno.h>
#include <geekos/kassert.h>
#include <geekos/ktypes.h>
#include <geekos/screen.h>  /* for debug Print() statements */
#include <geekos/pfat.h>
#include <geekos/malloc.h>
#include <geekos/string.h>
#include <geekos/user.h>
#include <geekos/elf.h>


/**
 * From the data of an ELF executable, determine how its segments
 * need to be loaded into memory.
 * @param exeFileData buffer containing the executable file
 * @param exeFileLength length of the executable file in bytes
 * @param exeFormat structure describing the executable's segments
 *   and entry address; to be filled in
 * @return 0 if successful, < 0 on error
 */
int Parse_ELF_Executable(char *exeFileData, ulong_t exeFileLength,
    struct Exe_Format *exeFormat)
{
    elfHeader * elfH =(elfHeader *) exeFileData;

    /** Primero determinamos si es un archivo ELF valido chequeando la informacion del campo 'ident' */
    if (((*elfH).ident[EI_MAG0]!= 0x7f)         ||/* Chequeamos los magic numbers */
        ((*elfH).ident[EI_MAG1]!='E')           ||
        ((*elfH).ident[EI_MAG2]!='L' )          ||
        ((*elfH).ident[EI_MAG3]!='F' )          ||
        ((*elfH).ident[EI_CLASS]!= ELFCLASS32)  ||/* Esperamos objeto de 32 bits*/
        ((*elfH).ident[EI_VERSION]!= EV_CURRENT)||/* La version debe ser la actual */
        ((*elfH).phnum > EXE_MAX_SEGMENTS) ||/* Tampoco debe haber mas segmentos de los permitidos */
        ((*elfH).type != ET_EXEC)           /* El archivo debe ser un ejecutable */
    )
        return -1;
    /** Comenzamos con el llenado de la variable 'exeFormat' */
    (*exeFormat).numSegments = (*elfH).phnum;
    (*exeFormat).entryAddr = (*elfH).entry;
    int i=0;
    while(i<((*elfH).phnum)){
        programHeader * ph;
        ph =(programHeader *)(exeFileData+(*elfH).phoff+i*(*elfH).phentsize);
        (*exeFormat).segmentList[i].offsetInFile = (*ph).offset;
        (*exeFormat).segmentList[i].lengthInFile = (*ph).fileSize;
        (*exeFormat).segmentList[i].startAddress = (*ph).vaddr;
        (*exeFormat).segmentList[i].sizeInMemory = (*ph).memSize;
        (*exeFormat).segmentList[i].protFlags = (*ph).flags;
        i++;
    }
    return 0;
    TODO("Parse an ELF executable image");
}

