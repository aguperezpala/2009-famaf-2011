/* * * * * * * * * * * * * * * * * * * * * * * *
 * <  TAD String  >--<  Versión de Mariano  >  *
 * * * * * * * * * * * * * * * * * * * * * * * */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "no_std_assert.h"



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
  int i = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (0 <= pos <= str->len_data);
  ASSERT (0 <= len);
  
  result = (char*) calloc (len + 1, sizeof (char));
  ASSERT (result != NULL);
    
  str->initialized = 1;
  str-> pos = pos;
  
  for (i = 0; i < len; i++)
    result[i] = str->data[(pos + i) % str->len_data];
  
  result[len] = '\0';
    
  return result;
}


char* string_get_back (string *str, int pos ,int len){
  int i = 0;
  int index = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (0 <= pos <= str->len_data);
  ASSERT (0 <= len);
  
  result = (char*) calloc (len + 1, sizeof (char));
  ASSERT (result != NULL);
      
  str-> initialized = 1;
  str-> pos = pos;
  
  index = pos % str-> len_data;
  for (i = 0; i < len; i++){
    result[(len - 1) - i] = str->data[index];
    index = (index - 1 + str-> len_data) % str-> len_data; /* No preguntar */
  }
  result[len] = '\0';
  
  return result;
}


char* string_slice_right (string *str, int count, int len){
  int i = 0;
  int pos = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (str-> initialized == 1);
  ASSERT (0 <= count <= str-> len_data);
  ASSERT (0 <= len);
  
  result = (char*) calloc (len + 1, sizeof (char));
  ASSERT (result != NULL);
    
  pos = str-> pos + count;
  
  for (i = 0; i < len; i++)
    result[i] = str->data[(pos + i) % str->len_data];
  
  result[len] = '\0';

  return result;
}


char* string_slice_left (string *str, int count, int len){
  int i = 0;
  int index = 0;
  int pos = 0;
  char* result = NULL;
  
  /* PRE: */
  ASSERT (str != NULL);
  ASSERT (str-> initialized == 1);
  ASSERT (0 <= count <= str-> len_data);
  ASSERT (0 <= len);
    
  result = (char*) calloc (len + 1, sizeof (char));
  ASSERT (result != NULL);

  pos = str->pos - count;

  index = pos % str-> len_data;
  for (i = 0; i < len; i++){
    result[(len - 1) - i] = str->data[index];
    index = (index - 1 + str-> len_data) % str-> len_data;
  }
  result[len] = '\0';
    
  return result;
}
