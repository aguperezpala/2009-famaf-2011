#ifndef STRING_ARQ_H
#define STRING_ARQ_H

/* las unicas librerias que vamos a usar */
#include <stdio.h>
#include <stdlib.h>



/* Definimos el tipo */
typedef struct _string string_t;


/* Constructor:
	REQUIRES:
		size > 0 (the buffer size)
		data != null && size(data) == size
	RETURNS:
		NULL => error
		string != NULL (!error)
*/
string_t * string_create (unsigned int size, char * data);


/* Destructor
	REQUIRES:
		string != NULL
*/
void string_destroy (string_t * string);





#endif
