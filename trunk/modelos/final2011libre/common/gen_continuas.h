#ifndef GEN_CONTINUAS_H
#define GEN_CONTINUAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "rdg.h"


/* Genera una variable aleatoria continua uniforme en el intervalo (a,b)
 * Funcion densidad de prob: f(x) = 1/(b-a) si a <= x <= b; 0 caso contrario.
 * Funcion distribucion de prob: F(x) = 0 (si x < a); (x-a)/(b-a) a <= x <= b;  1 si x >= b;
*/ 
double rg_gen_uni_cont(double a, double b);


#endif
