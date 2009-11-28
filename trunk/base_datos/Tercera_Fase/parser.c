/*! NOTE: gcc -Wall -ansi -pedantic -o parser parser.c */

#define _GNU_SOURCE


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_FIELD_SIZE		100
#define CHAR_FIELD_SEPARATOR	','	/* como estan separados los campos en archivos */
#define TABLE_NAME	"planilla"

typedef struct {
	FILE *file;
	char *fileName;
	int fields[50];		/*! kakaso */
	char *scapeChars;	/* aca determinamos que caracter va a encerar
				 * el valor, por ejemplo si el campo 1 del archivo
				 * es del tipo int, entonces scapeChars[0] = ' '
				 * y del mismo archivo el campo 2 es char entonces
				 * scapeChars[1] = '\''. osea tendriamos
				 * scapeChars = " \'";
				 */
} tuple_t;


/*! los fields deben empezar en 0, son las posiciones de los elementos
 * que hay que sacar del archivo
 * ej: field0,field1,field2,etc\n
 * NOTE: Siempre debemos terminar con -1 en el arreglo de fields
*/
tuple_t map[] = {{NULL, "planilla-vivienda.txt", 
	{0,520,521,529,522,521,527,525,528,527,523,556,555,2,510,511,1,520,520,520,520,-1},
	"             \'  \'"},
	/*! comienza campo 22 */
	{NULL, "planilla-transporte.txt", 
	{512,512,512,512,1,521,-1},
	"    \' "},
	/*! comienza campo 28 */
	{NULL, "planilla-electrico.txt", 
	{1,-1},
	"\'"},
	/*! comienza campo 29 */
	{NULL, "planilla-telefono.txt", 
	{1,-1},
	"\'"},
	/*! comienza campo 30 */
	{NULL, "planilla-informacion.txt", 
	{1,-1},
	"\'"},
	/*! comienza campo 31 */
	{NULL, "planilla-salud.txt", 
	{1, 2,555,555,555,555,555,-1},
	"\'\'     "},
	/*! comienza campo 38 */
	{NULL, "planilla-trabajo.txt", 
	{521,521,521,4,1,2,3,-1},
	"    \'\'\'"},
	/*! comienza campo 28 */
	{NULL, "planilla-electrico.txt", 
	{1,-1},
	"\'"},
	/*! comienza campo 28 */
	{NULL, "planilla-electrico.txt", 
	{1,-1},
	"\'"},
	/*! comienza campo 28 */
	{NULL, "planilla-electrico.txt", 
	{1,-1},
	"\'"},
	
	{NULL, NULL, {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}, NULL}
		};




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
/* funcion que carga la estructura map.
 * REQUIRES:
 * 	m != NULL
 * RETURNS:
 * 	0	en caso exitoso
 * 	< 0	en caso de error
*/
int load_map(tuple_t * m)
{
	int i = 0;
	
	assert(m != NULL);
	
	while(m[i].fileName != NULL) {
		m[i].file = fopen(m[i].fileName, "r");
		if(m[i].file == NULL){
			printf("Error al abrir el archivo %s\n", m[i].fileName);
			return -1;
		}
		i++;
	}
	
	return 0;
}

/* Funcion que cierra toda la estructura de map
 * REQUIRES:
 * 	m != NULL
 */
void unload_map(tuple_t * m)
{
	int i = 0;
	
	assert(m != NULL);
	
	while(m[i].file != NULL) {
		fclose(m[i].file);
		i++;
	}
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
	int i = 0, j = 0;
	bool isFinish = false;
	char result[500];
	char *field = NULL;
	
	/* cargamos las estructuras */
	if (load_map(map) < 0) {
		printf("Error al cargar un archivo\n");
		return 1;
	}
	
	memset(result, '\0', 500);
	/* iteramos ahora */
	while(!isFinish) {
		/* limpiamos */
		memset(line, '\0', 500);
		/* obtenemos la linea */
		if (read_line(line, map[i].file) < 0) {
			/* NOTE:si estamos en el ultimo archivo es porque debemos
			 * salir, ESTO ES PORQUE SI O SI DEBE HABER UNA 
			 * CORRESPONDENCIA DE CANTIDAD DE TAMAÑOS DE ARCHIVOS
			 * (TODOS LOS ARCHIVOS TIENEN EL MISMO TAMAÑO)
			 */
			if(map[i+1].file == NULL) {
				isFinish = true;
			}
			
			i++;
			continue;
		}
		/* ahora calculamos los campos*/
		j = 0;
		while (map[i].fields[j] >= 0) {
			field = NULL;
			field = get_field(line, map[i].fields[j]);
			if (field == NULL) {
				printf("Error al recibir un campo en archivo "
				      "%s, linea:%s, campo: %d\n", 
	  				map[i].fileName, line, j);
				j++;
				continue;
			}
			/* hacemos un append al result con los scapeChars
			* especificados en m[i].scapeChars
			*/
			result[strlen(result)] = map[i].scapeChars[j];
			strcat(result, field);
			result[strlen(result)] = map[i].scapeChars[j];
			/* agregamos una coma para separar cada valor */
			result[strlen(result)] = ',';
			
			/* liberamos */
			free(field); field = NULL;
			j++;
		}
		
		/* ahora vamos al prox archivo que tenemos que leer */
		if (map[i+1].file == NULL) {
			/*! Aca en result tenemos todas los valores, hacemos un 
		 	* cochino printf de pecho, pero antes hacemos otra cochinada
			*/
			/*if (result[strlen(result)] == ',')*/
			/*printf("strlen(result): %d\n",(int)strlen(result));*/
			result[strlen(result) - 1] = '\0';
			
			printf("INSERT INTO \"%s\" VALUES(%s);\n", TABLE_NAME, result);
			memset(result, '\0', 500);
			i = 0;
		}
		else
			i++;
		
	}
	
	/* descargamos la estructura */
	unload_map(map);
	
	return 0;
}

