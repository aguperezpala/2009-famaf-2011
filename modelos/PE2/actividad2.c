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
	double auxArr[size];
	
	assert(arr != NULL);
	
	/* lo ordenamos */
	memcpy(auxArr, arr, size * sizeof(*arr));
	sort_doubles(auxArr,size);
	
	if((size % 2) == 0) {
		/* es par != devolvemos el promedio de los 2 valores del medio */
		i = ((int) size/2);
		return ((auxArr[i - 1] + auxArr[i])/(double) 2.0);
	} else {
		/* es impar => devolvemos simplemente el valor del medio */
		i = (int) size/2;
		return auxArr[i];
	}
	return -1;
}

/* Funcion que calcula el primer cuartil (q1) dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	q1(arr)
*/
double act2_get_q1(double *arr, int size)
{
	int i = 0;
	double auxArr[size];
	
	assert(arr != NULL);
	
	/* lo ordenamos */
	memcpy(auxArr, arr, size * sizeof(*arr));
	sort_doubles(auxArr,size);
	if (size % 2 == 0)
		size = size/2;
	else
		size = (size/2) + 1;
	
	return act2_get_mediana(auxArr, size);
}

/* Funcion que calcula el segundo cuartil (q3) dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	q3(arr)
*/
double act2_get_q3(double *arr, int size)
{
	int i = 0;
	double auxArr[size];
	double *arrPtr = NULL;
	
	
	assert(arr != NULL);
	
	/* lo ordenamos */
	memcpy(auxArr, arr, size * sizeof(*arr));
	sort_doubles(auxArr,size);
	
	if (size % 2 == 0){
		size = size/2;
		arrPtr = &auxArr[size];
	} else {
		size = (size/2) + 1;
		arrPtr = &auxArr[size - 1];
	}
	
	return act2_get_mediana(arrPtr, size);
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

/* Funcion que calcula la varianza dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	V(arr)
*/
double act2_get_varianza(double *arr, int size)
{
	double media = 0;
	double varianza = 0;
	int i = 0;
	
	assert(arr != NULL);
	
	media = act2_get_media(arr, size);
	for(i = 0; i < size; i++)
		varianza += pow(arr[i] - media, 2.0)/(double) (size - 1);
	
	return varianza;
}

/* Funcion que calcula la skewness dado un arreglo:
* REQUIRES:
* 	arr 	!= NULL
* 	n 	= size(arr)
* RETURNS:
* 	skewness(arr)
*/
double act2_get_skewness(double *arr, int size)
{
	double 	media = 0,
		varianza = 0,
		sum1 = 0;
	int i = 0;
	double result = 0;
	
	assert(arr != NULL);
	
	media = act2_get_media(arr, size);
	varianza = act2_get_varianza(arr, size);
	
	
	for(i = 0; i < size; i++)
		sum1 += pow(arr[i] - media, 3)/(double)size;
	
	result = sum1/(pow(varianza, 1.5));
	
	return result;
}


