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


typedef double(*FUNC_PROM_PTR)(double);



/* Generacion de permutaciones equiprobables */
void permutar_equiprobable(int *arr, int size);

/* permutacion no equiprobable */
void permutar_no_equiprobable(int *arr, int size);

/* Calculo de promedios para a = Sumatoria a(i)/n from i=1 to n, n >> 1
 * podemos simularlo calculando la E(a(X)) con Xi~floor(n*Ui)+1 pero iterando
 * solo k veces con k < n.
 * NOTE: TENER CUIDADO CON ESTA FUNCION, fijarse de implementar por nuestra
 * cuenta (mirar problema2 del practico 4).
 */
double calc_prom(int n, int k, FUNC_PROM_PTR a);


/* Funcion que ordena un arreglo de doubles, cambiar el tipo si necesitamos en
 * orden decreciente a[0] < a[1] < ... < a[size] */
void sort_doubles(double *arr, int size);


#endif
