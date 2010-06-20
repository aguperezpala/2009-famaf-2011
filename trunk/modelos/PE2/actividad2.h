#ifndef ACTIVIDAD2_H
#define ACTIVIDAD2_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

/* Funcion que calcula el maximo dado un arreglo:
 * REQUIRES:
 * 	arr 	!= NULL
 * 	n 	= size(arr)
 * RETURNS:
 * 	max(arr)
 */
double act2_get_max(double *arr, int size);

/* Funcion que calcula el minimo dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	min(arr)
*/
double act2_get_min(double *arr, int size);

/* Funcion que calcula la mediana dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	mediana(arr)
*/
double act2_get_mediana(double *arr, int size);

/* Funcion que calcula el primer cuartil (q1) dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	q1(arr)
*/
double act2_get_q1(double *arr, int size);

/* Funcion que calcula el segundo cuartil (q3) dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	q3(arr)
*/
double act2_get_q3(double *arr, int size);

/* Funcion que calcula la media dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	media(arr)
*/
double act2_get_media(double *arr, int size);

/* Funcion que calcula la varianza dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	V(arr)
*/
double act2_get_varianza(double *arr, int size);

/* Funcion que calcula la skewness dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	skewness(arr)
*/
double act2_get_skewness(double *arr, int size);


#endif
