#include "actividad2.h"

/* Algoritmo que ordena un arreglo de doubles */
static void sort_doubles(double *arr, int size) 
{
	int a = 0, b = 0;
	double t = 0;
	
	for(a = 1; a < size; ++a)
		for(b = size-1; b >= a; --b) {
			if(arr[ b - 1] > arr[ b ]) {
				t = arr[ b - 1];
				arr[ b - 1] = arr[ b ];
				arr[ b ] = t;
			}
		}
}



/* Funcion que calcula el maximo dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	max(arr)
*/
double act2_get_max(double *arr, int size)
{
	double max = 0;
	int i = 0;
	
	assert(arr != NULL);
	
	max = arr[0];
	for(i = 0; i < size; i++)
		if(arr[i] > max)
			max = arr[i];
		
	return max;
}

/* Funcion que calcula el minimo dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	min(arr)
*/
double act2_get_min(double *arr, int size)
{
	double min = 0;
	int i = 0;
	
	assert(arr != NULL);
	
	min = arr[0];
	for(i = 0; i < size; i++)
		if(arr[i] < min)
			min = arr[i];
		
		return min;
}


/* Funcion que calcula la mediana dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	mediana(arr)
*/
double act2_get_mediana(double *arr, int size)
{
	int i = 0;
	
	assert(arr != NULL);
	
	/* lo ordenamos */
	sort_doubles(arr,size);
	
	if((size % 2) == 0) {
		/* es par != devolvemos el promedio de los 2 valores del medio */
		i = ((int) size/2);
		return ((arr[i - 1] + arr[i])/(double) 2.0);
	} else {
		/* es impar => devolvemos simplemente el valor del medio */
		i = (int) size/2;
		return arr[i];
	}
	return -1;
}


/* Funcion que calcula la media dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	media(arr)
*/
double act2_get_media(double *arr, int size)
{
	double media = 0;
	int i = 0;
	
	assert(arr != NULL);
	
	for(i = 0; i < size; i++)
		media += arr[i];
	
	media = media / (double)size;
	
	return media;
}



