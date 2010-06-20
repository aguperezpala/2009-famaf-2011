#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


/* Funcion que lee un archivo de texto y los carga en un arreglo de tamaño
 * fijo.
 * REQUIRES:
 * 	n	size of array
 * 	arr	array != NULL
 * 	fname	fileName != NULL
 * RETURNS:
 * 	< 0 		on error
 * 	lines readed	if success
 */
int read_double_file(double *arr, int size, const char *fname);


#endif
