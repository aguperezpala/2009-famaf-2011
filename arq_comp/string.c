/* * * * * * * * * * * * * * * * * * * * * * * *
 * <  TAD String  >--<  Versión de Mariano  >  *
 * * * * * * * * * * * * * * * * * * * * * * * */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "string.h"



string* string_create (char* cstr){
  string *result = NULL;
  
  result = (string*) calloc (1, sizeof (*result));
  
  if (result != NULL){
    result-> data = cstr;
    result-> len_data = strlen (cstr);
    result-> pos =-1; 
    result-> initialized = 0;
  }
  
  return result;
}

void string_destroy (string *str){
  free (str);
}

char* string_get_front (string *str, int pos ,int len){
  int i = 0;
  char* result = NULL;
  
  if (pos >= 0 && len >= 0){
    result = (char*) calloc (len + 1, sizeof (char));
    
    if (result != NULL){
    	str->initialized = 1;
	str-> pos = pos;
	
	for (i = 0; i < len; i++)
	  result[i] = str->data[(pos + i) % str->len_data];
	
	result[len] = '\0';
    }
  
  }
  
  return result;
}

char* string_get_back (string *str, int pos ,int len){
  int i = 0;
  int index = 0;
  char* result = NULL;
  
  if (pos >= 0 && len >= 0){
    result = (char*) calloc (len + 1, sizeof (char));
      
    if (result != NULL){
      str->initialized = 1;
      str-> pos = pos;
      
      index = pos % str-> len_data;
      for (i = 0; i < len; i++){
	result[(len - 1) - i] = str->data[index];
	index = (index - 1 + str-> len_data) % str-> len_data;
      }
      result[len] = '\0';
    }
  }
  
  return result;
}

char* string_slice_right (string *str, int count, int len){
  int i = 0;
  int pos = 0;
  char* result = NULL;
  
  if (count >= 0 && len >= 0 && str-> initialized == 1){
    result = (char*) calloc (len + 1, sizeof (char));
    
    if (result != NULL){   
      pos = str-> pos + count;
      
      for (i = 0; i < len; i++)
	result[i] = str->data[(pos + i) % str->len_data];
      
      result[len] = '\0';
    }
  }
  
  return result;
}

char* string_slice_left (string *str, int count, int len){
  int i = 0;
  int index = 0;
  int pos = 0;
  char* result = NULL;
  
  if (pos >= 0 && len >= 0 && str->initialized == 1){
    result = (char*) calloc (len + 1, sizeof (char));
      
    if (result != NULL){
      
      pos = str->pos - count;
      
      index = pos % str-> len_data;
      for (i = 0; i < len; i++){
	result[(len - 1) - i] = str->data[index];
	index = (index - 1 + str-> len_data) % str-> len_data;
      }
      result[len] = '\0';
    }
  }
  
  return result;
}