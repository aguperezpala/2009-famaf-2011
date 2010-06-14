#ifndef ACTIVIDAD4_H
#define ACTIVIDAD4_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "actividad2.h"
#include "distrib.h"

/* Funcion que devuelve el p-valor utilizando la aproximacion ji-cuadrada.
 * REQUIRES:
 * 	k	= num de segmentos
 * 	arr	= arreglo de valores de la muestra
 * 	size	= tamaño de la muestra
 * 	fun	= H0 (puntero a funcion "double func(double)")
 * 	gl	= grados de libertad
 * RETURNS:
 * 	p-valor
 */
double get_p_value_ji(int k, double *arr, int size, double (*fun)(double), int gl);

/* Funcion que devuelve el p-valor utilizando la aproximacion kolmogorov-Smirnov.
* REQUIRES:
* 	k	= num de segmentos
* 	arr	= arreglo de valores de la muestra
* 	size	= tamaño de la muestra
* 	Fun	= H0 (puntero a funcion "double func(double)")NOTE: FH0(acumulada)
* 	gl	= grados de libertad
* RETURNS:
* 	p-valor
*/
double get_p_value_ks(int k, double *arr, int size, double (*Fun)(double), int gl);






#endif
