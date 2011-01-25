#ifndef ALGORITMOS_EXTRAS_H
#define ALGORITMOS_EXTRAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "rdg.h"
#include "gen_continuas.h"
#include "gen_discretas.h"


typedef double (*FUNC_PROM_PTR)(double);



/* Generacion de permutaciones equiprobables */
void permutar_equiprobable(int *arr, int size);

/* permutacion no equiprobable */
void permutar_no_equiprobable(int *arr, int size);

/* Calculo de promedios para a = Sumatoria a(i)/n from i=1 to n, n >> 1
 * podemos simularlo calculando la E(a(X)) con Xi~floor(n*Ui)+1 pero iterando
 * solo k veces con k < n.
 */
double calc_prom(int n, int k, FUNC_PROM_PTR a);

#endif
