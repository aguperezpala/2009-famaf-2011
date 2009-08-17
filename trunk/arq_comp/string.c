#include "string.h"


struct _string {
	unsigned int size;	/* el tamaño del string */
	unsigned int p;		/* el puntero del visor */
	char * data;		/* los datos */
};





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



/* ### GETS ### */

/* Funcion que devuelve un string_t * de winSize caracteres, comenzando desde 
* string[posOff] hacia adelante.
* 	REQUIRES:
* 		string != NULL
*		winSize >= 0
*		0 <= posOff <= size(string)
*	RETURNS:
*		NULL => error
*		new string_t * => no error
*/
string_t * string_get_front (string_t * string, unsigned int winSize, unsigned int posOff);


/* Funcion que devuelve un string_t * de winSize caracteres, comenzando desde 
* string[posOff] hacia atras.
* 	REQUIRES:
* 		string != NULL
*		winSize >= 0
*		0 <= posOff <= size(string)
*	RETURNS:
*		NULL => error
*		new string_t * => no error
*/
string_t * string_get_back (string_t * string, unsigned int winSize, unsigned int posOff);


/* Funcion que devuelve el tamaño del string
*	REQUIRES:
*		string != NULL
*	RETURNS:
*		stringSize
*/
unsigned int string_get_size (string_t * string);


/* Funcion que devuelve la posicion del "visor"
*	REQUIRES:
*		string != NULL
*	RETURNS:
*		viewerPos
*/
unsigned int string_get_viewer_pos (string_t * string);


/* Funcion que devuelve el caracter de la posicion actual del visor
*	REQUIRES:
*		string != NULL
*	RETURNS:
*		char
*/
char string_get_char (string_t * string);


/* ### set ### */


/* Funcion que setea un caracter en la posicion del "visor"
*	REQUIRES:
*		string != NULL
*		c = caracter
*/
void string_set_char (string_t * string, char c);






/* ### Slides ### */

/* Funcion que mueve el "visor" de un string_t hacia adelante desp lugares, en 
* caso de desplazarse mas alla del tamaño, comienza nuevamente (osea se mueve
* sobre el arreglo en forma "circular")
* 	REQUIRES:
* 		string != NULL
*		winSize >= 0
*		desp >= 0
*//* FIXME: que hace el winSize? */
void string_slide_front (string_t * string, unsigned int winSize, unsigned int desp);


/* Funcion que mueve el "visor" de un string_t hacia atras desp lugares, en 
* caso de desplazarse mas alla del tamaño, comienza nuevamente (osea se mueve
* sobre el arreglo en forma "circular")
* 	REQUIRES:
* 		string != NULL
*		winSize >= 0
*		desp >= 0
*//* FIXME: que hace el winSize? */
void string_slide_back (string_t * string, unsigned int winSize, unsigned int desp);




/* ### Adicionales ### */

/* Funcion que sirve para concatenar 2 string_t, devolviendo el resultado en
*  un nuevo string_t * (NOTE: alloca memoria)
*	REQUIRES:
*		string1 != NULL
*		string2 != NULL
*	RETURNS:
*		(string_t *) string1 ++ string2
*/
string_t * string_concat (string_t * string1, string_t * string2);
