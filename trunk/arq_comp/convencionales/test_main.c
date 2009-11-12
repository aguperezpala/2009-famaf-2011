#include <stdio.h>
#include <stdlib.h>

#include "win_string.h"
#include "no_std_assert.h"

#define POS 1
#define LEN 6

int main () {

  char* cstr = NULL;
  char* result = NULL;
  string *str = NULL;
  
  cstr = "Zapallo";
  printf ("-- %s --\n",cstr);
  
  str = string_create (cstr);  
  ASSERT (str != NULL);
  
  
  
  
  
  result = string_slice_right (str, 0, 6);
  printf ("%s \n",result);
  free (result);
  
  
  
  
  
  
  
  
  
  

  return 0;
}