/* * * * * * * * * * * * * * * * * * * * * * * *
 * <  TAD String  >--<  Versión de Mariano  >  *
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "win_string.h"
#include "no_std_assert.h"

/*
 * Función principal, notar que todas se definen a partir
 * de ésta. De esta forma no repetimos código.
 * Se diferencian una a otra, por la posición de donde
 * se toma el 'string_pointer', si se refresca éste o no
 * y si inicializa o no al string.
 * De acuerdo a la definición se setean los argumentos de
 * refresh_sp y init.
 */
static char* _string_get_front (string *str, int pos,  int len, 
                                int refresh_sp, int init){
  int i = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (0 <= pos && pos < str->len_data);
  ASSERT (0 <= len);
  
  result = (char*) calloc (len + 1, sizeof (char));
  ASSERT (result != NULL);
  
  if (init) str->initialized = 1;
  
  if (refresh_sp) str-> pos = pos;
  
  for (i = 0; i < len; i++)
    result[i] = str->data[(pos + i) % str->len_data];
  
  result[len] = '\0';
    
  return result;
}

string* string_create (char* cstr){
  string *result = NULL;
  
  /* PRE: */
  ASSERT (cstr != NULL);
  
  result = (string*) calloc (1, sizeof (*result));
  ASSERT (result != NULL);
  
  result-> data = cstr;
  result-> len_data = strlen (cstr);
  result-> pos =-1; 
  result-> initialized = 0;
  
  return result;
}


string* string_destroy (string *str){
  free (str);
  return NULL;
}


char* string_get_front (string *str, int pos ,int len){
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (0 <= pos && pos < str->len_data);
  ASSERT (0 <= len);
  
  result = _string_get_front (str, pos, len, 1, 1);
  
  return result;
}


char* string_get_back (string *str, int pos , int len){
  int tmp = 0; 
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (0 <= pos && pos < str-> len_data);
  ASSERT (0 <= len);
  
  str-> pos = pos;
  
  /* Ver definición de strin_get_back () */
  tmp = pos - len + 1;
  
  /* tmp puede ser negativo */
  tmp = (tmp + str-> len_data) % str-> len_data;
  
  result = _string_get_front (str, tmp, len, 1, 1);
      
  return result;
}


char* string_slice_right (string *str, int count, int len){
  int i = 0;
  int pos = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (str-> initialized == 1);
  ASSERT (0 <= count && count <= str-> len_data);
  ASSERT (0 <= len);
  
  result = _string_get_front (str, count, len, 0, 0);

  return result;
}


char* string_slice_left (string *str, int count, int len){
  int tmp = 0;  
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (str-> initialized == 1);
  ASSERT (0 <= count && count <= str-> len_data);
  ASSERT (0 <= len);
  
  tmp = count - len + 1;
  tmp = (tmp + str-> len_data) % str-> len_data;
    
  result = _string_get_front (str, tmp, len, 0, 0);
    
  return result;
}
