#ifndef ACTIVIDAD2_H
#define ACTIVIDAD2_H

#include <stdio.h>
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

/* Funcion que calcula la media dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	media(arr)
*/
double act2_get_media(double *arr, int size);



#endif
