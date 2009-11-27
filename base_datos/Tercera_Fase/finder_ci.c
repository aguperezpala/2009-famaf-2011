/*! NOTE: gcc -Wall -ansi -pedantic -o finder_ci finder_ci.c */

#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>


#define MAX_FIELD_SIZE		100
#define CHAR_FIELD_SEPARATOR	','	/* como estan separados los campos en archivos */
/* Funcion que lee una linea y la guarda en un buffer
 * REQUIRES:
 * 	buff != NULL
 * 	file != NULL
 * RETURNS:
 * 	< 0 		en caso de error
 * 	readBytes	caso contrario
 */
int read_line(char *buff, FILE *file)
{
	int result = -1, i = 0;
	int c = 0;
	
	assert(buff != NULL);
	assert(file != NULL);
	
	c = fgetc(file);
	while(c != EOF && c != '\n') {
		buff[i] = c;
		i++;
		c = fgetc(file);
	}
/* 	buff[i] = '\n';
 	i++;*/
	buff[i] = '\0';
	if (i > 0)
		result = i;
	
	
	return result;
}


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
	char *cPointer = line;
	int cPos = 0;
	char *startPos = NULL, *endPos = NULL;
	
	assert(line != NULL);
	
	result = calloc (MAX_FIELD_SIZE + 1, sizeof(*result));
	assert (result != NULL);
	memset(result, '\0', MAX_FIELD_SIZE + 1);
	
	/*! devolvemos NULL como string */
	if (fieldPos > 500) {
		strcpy(result, "NULL");
		return result;
	}
		
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
	if ((endPos = strchr(startPos, CHAR_FIELD_SEPARATOR)) == NULL)
		/* entonces hay que buscar un \n */
		endPos = strchr(startPos, '\0');
	/* hay error? */
	if (endPos == NULL) {
		free(result);
		result = NULL;
		return result;
	}
	/* todo joya */
	/*printf("startPos: %s\nendPos: %s\n",startPos, endPos);*/
	memcpy(result, startPos, ((int) (endPos - startPos)));
	
	return result;
}


int main(void)
{
	char line[500];
	bool isFinish = false, notFound = true;
	char result[500];
	char *field = NULL, *field2 = NULL;
	FILE *fFamilia = NULL, *fJefe = NULL;
	
	
	fFamilia = fopen("planilla-familias.txt", "r");
	
	memset(result, '\0', 500);
	/* iteramos ahora */
	while(!isFinish) {
		/* limpiamos */
		memset(line, '\0', 500);
		if (read_line(line, fFamilia) < 0) {
			isFinish = true;
			continue;
		}
		field = get_field(line, 0);
		notFound = true;
		
		fJefe = fopen("planilla-jefe_familia.txt", "r");
		if (!fJefe)
			assert(false);
		memset(line, '\0', 500);
		
		while (!isFinish && notFound && read_line(line, fJefe) >= 0) {
			field2 = get_field(line, 0);
			
			if (strcmp(field,field2) == 0) {
				/* son iguales entonces imprimimos el numero
				 * de planilla */
				free(field2);
				/* obtenemos la planilla numero */
				field2 = get_field(line,1);
				printf("%s\n",field2);
				notFound = false;
			}
			free(field2);
		}
		
		fclose(fJefe); fJefe = NULL;
		free(field);
		
	}
	fclose (fFamilia);
	
	return 0;
}

