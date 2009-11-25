#define __GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_FIELD_SIZE		100
#define CHAR_FIELD_SEPARATOR	','

typedef struct {
	FILE *file;
	char *fileName;
	int *fields;	
} tuple_t;


/*! los fields deben empezar en 0, son las posiciones de los elementos
 * que hay que sacar del archivo
 * ej: field0,field1,field2,etc\n
*/
tuple_t *map = {{NULL, "archivo1.sql", {5}},
		{NULL, "archivo2.sql", {2,3}},
  		{NULL, NULL, NULL}
		};

/* Funcion que va a devolver un char * si pudo obtener bien el dato
 * que va a ser la linea actual del archivo el campo numero fieldPos
 * REQUIRES:
	line != NULL
	fieldPos > 0
 * RETURNS:
	field 	si no hubo error
	NULL	si hubo error
*/
char * get_field(char * line, int fieldPos)
{
	char *result = NULL;
	char *cPointer= line;
	int cPos = 0;
	char *startPos = NULL, *endPos = NULL;
	
	assert(line != NULL);
	
	result = calloc (MAX_FIELD_SIZE + 1, sizeof(*result));
	assert (result != NULL);
	memset(result, '\0', MAX_FIELD_SIZE + 1);
	
	/* ahora parseamos el campo determinado */
	
	while(cPos < fieldPos) {
		/* vamos a avanzar hasta el campo que queremos */
		if ((cPointer = strchr(cPointer, CHAR_FIELD_SEPARATOR)) == NULL)
			/* error ? */
			break;
		else {
			cPointer++; /* salteamos el CHAR_FIELD_SEPARATOR */
			cPos++;	/* encontramos uno asique aumentamos */
		}
	}
	/* hubo error? */
	if (cPos != fieldPos) {
		free(result);
		result = NULL;
		return result;
	}
	/* no hubo error... obtenemos el string correspondiente */
	startPos = cPointer;
	/* buscamos el final, pero puede que sea un \n */
	if ((endPos = strchar(cPointer, CHAR_FIELD_SEPARATOR)) == NULL)
		/* entonces hay que buscar un \n */
		endPos = strchar(cPointer, '\n');
	/* hay error? */
	if (endPos == NULL) {
		free(result);
		result = NULL;
		return result;
	}
	/* todo joya */
	memcpy(result, startPos, ((int) (endPos - startPos)) - 1);
	
	return result;
}


int main(void)
{
	char line[500];
	
	
	