#include <stdio.h>
#include <stdlib.h>

/*#include "win_string.h"
#include "no_std_assert.h"*/

int main () {

  char* cstr = NULL;
  char* result = NULL;
  int j = 0;
  
  /*string *str = NULL;*/
  
  cstr = "Zapallo";
  
  for (j = 0; j < 7; j++)
    printf ("%i\n", cstr[j]);
  
  /*
  
    
  str = string_create (cstr);  
  ASSERT (cstr != NULL);
  
  result = string_get_back (str, 4, 6);
  printf ("%s \n",result);
  free (result);
  
  result = string_slice_right (str, 0, 6);
  printf ("%s \n",result);
  free (result);
  
  
  
  
  
  */
  
  
  
  

  return 0;
}