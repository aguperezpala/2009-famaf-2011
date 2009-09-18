/* * * * * * * * * * * * * * * * * * * * * * * *
 * <  TAD String  >--<  Versión de Mariano  >  *
 * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef _STRING__
#define _STRING__

#include "no_std_assert.h"

typedef struct {
  char* data;
  int pos;
  int len_data;
  int initialized;
} string;

/* Crea un nuevo objeto 'string' a partir de 'cstr'
 * PRE: cstr != NULL
 *      cstr debe ser NUL terminado
 *  Notas: 'result' es dueño de 'cstr'
 */	
string* string_create (char* cstr);

/* Destruye el objeto string 'str'
 * PRE: str != NULL
 * Notas

void string_destroy (string *str);

char* string_get_front (string *str, int pos ,int len);

char* string_get_back (string *str, int pos ,int len);

char* string_slice_right (string *str, int count, int len);

char* string_slice_left (string *str, int count, int len);




#endif