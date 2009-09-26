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
	elfHeader *encabezado;
	programHeader *segmento;
	int i;

	/*Localizamos el encabezado del ELF, y nos fijamos si esta completo*/
	if (exeFileLength < sizeof(elfHeader)) {
		/*Unexpected end-of-file*/
		return -1;
	}
	encabezado = (elfHeader *) exeFileData;

	/*Chequeamos que el numero magico sea el correcto*/
	if (strncmp(encabezado->ident, ELF_MAGIC, sizeof(ELF_MAGIC)-1)) {
		return -18;
	}

	/*Llenamos los datos esenciales de la estructura exeFormat*/
	if (encabezado->phnum > EXE_MAX_SEGMENTS) {
		/*Too many segments*/
		return -2;
	}
	exeFormat->numSegments = encabezado->phnum;
	exeFormat->entryAddr = encabezado->entry;

	/*Ahora, iteramos sobre los encabezados de programa dentro del ELF*/
	for (i = 0; i < encabezado->phnum; i++) {

		/*Nos fijamos que este completo...*/
		if (encabezado->phoff + (i+1)*encabezado->phentsize > exeFileLength) {
			/*Unexpected end-of-file*/
			return -1;
		}
		segmento = (programHeader *) (exeFileData + encabezado->phoff + i*encabezado->phentsize);
		
		/*...y llenamos los datos para cada segmento correspondiente*/
		(exeFormat->segmentList[i]).offsetInFile = segmento->offset;
		(exeFormat->segmentList[i]).lengthInFile = segmento->fileSize;
		(exeFormat->segmentList[i]).startAddress = segmento->vaddr;
		(exeFormat->segmentList[i]).sizeInMemory = segmento->memSize;

		/*Esto queda deshabilitado hasta que no tengamos memoria virtual*/
		/*if (segmento->flags & PF_R) exeFormat->segmentList[i].protFlags |= VM_READ; 
		if (segmento->flags & PF_W) exeFormat->segmentList[i].protFlags |= VM_WRITE; 
		if (segmento->flags & PF_X) exeFormat->segmentList[i].protFlags |= VM_EXEC; */
	}

	return 0;

}

