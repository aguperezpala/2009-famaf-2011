/* * * * * * * * * * * * * * * *
 * * * * * TAD STRING * * * * *
 * * * * * * * * * * * * * * */
 
#ifndef _STRING__
#define _STRING__

typedef struct {
  char* data;
  int pos;          /* string_pointer */
  int len_data;
  int initialized;
} string;


/*
 * Crea un nuevo objeto 'string' a partir de 'cstr'
 * PRE: cstr != NULL
 *      cstr debe ser NUL terminado
 * Notas: 'result' es dueño de 'cstr'
 */	
string* string_create (char* cstr);


/*
 * Destruye el objeto string 'str'
 * PRE: str != NULL
 * Notas: 'cstr' queda a cargo del llamador
 */
string* string_destroy (string *str);


/*
 * Establece el 'string_pointer' en 'str' en la posición 'pos' y devuelve 
 * un puntero 'result' a un bloque de memoria de tamaño 'len' tal que
 * result [i] == str-> data[pos + i % data-> len_data] i = 0..len-1
 * PRE: str != NULL;
 *      0 <= pos < str->len_data
 *	0 <= len
 * POS: result != NULL
 *      result es NUL terminado 
 *      result debe ser liberado por el llamador
 *      result-> pos = pos                  (string_pointer)
 */
char* string_get_front (string *str, int pos ,int len);


/* 
 * Se define como:
 * string_get_front (str, tmp, len)
 * donde len = ((len - pos + 1) + len_data) % len_data
 * Ejemplo:
 *
 * pos = j
 * len = len
 *
 * str = A0 A1 A2 A3 A4 A5 A6 .. Aj .. An
 *
 * len (result) = len
 * result = [Aj-len + 1 .. Aj]
 */
char* string_get_back (string *str, int pos ,int len);


/* Idem a 'string_get_* ()' sólo que no modifican el 'string_pointer' 
 * PRE: str != NULL;
 *      0 <= pos < str->len_data
 *	0 <= len
 *      str-> initialized == 1
 * POS: result != NULL
 *      result es NUL terminado 
 *      result debe ser liberado por el llamador
 */
char* string_slice_right (string *str, int count, int len);

char* string_slice_left (string *str, int count, int len);

#endif
