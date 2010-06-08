/* Funciones de generación de valores de variables aleatorias con
 * distribuciones de probabilidad específicas
 * 
 * Última revisión: 03/05/10
 * 
 * V.A. implementadas:
 * 	Discretas:
 * 		· Uniforme (a,b)
 * 		· Poisson (lambda)
 * 	Continuas
 * 		· Uniforme (a,b)
 * 		· Exponencial (lambda)
 */

#ifndef RANDOM_GENERATORS_H
#define RANDOM_GENERATORS_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>


/* Funcion que genera una variable aleatoria de Poisson con parametro
 * lambda */
int rg_gen_poisson(double lambda);

/* generadora de una v.a. exponencial con parametro lambda */
double rg_gen_exp(double lambda);

/* funcion que genera una v.a. uniforme discreta en el intervalo [a,b] */
int rg_gen_uni_dis(int a, int b);

/* funcion que genera una v.a. uniforme continua en (a,b) */
double rg_gen_uni_cont(double a, double b);


#endif
