#include <stdio.h>
#include <stdlib.h>

#define ASSERT(s)  !s ? abort () : 1

int main () {

  ASSERT (1);
  printf ("Hola!\n");
  
  

  return 0;
}