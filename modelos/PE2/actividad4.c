#include "actividad4.h"

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
double get_p_value_ji(int k, double *arr, int size, double (*fun)(double), int gl)
{
	int N[k] = {0};
	double 	min = 0,
		max = 0,
		delta = 0,
		T = 0,
		result = 0;
		
	int i = 0, j = 0;
	
	assert(arr != NULL);
	assert(fun != NULL);
	assert(size > 0 && k >= 0);
	
	/* calculamos el tamaño de los intervalos */
	min = act2_get_min(arr, size);
	max = act2_get_max(arr, size);
	delta = (max - min)/(double)k;
	
	/* llenamos los Ni ineficientemente */
	for(i = 0; i < k; i++)
		for(j = 0; j < size; j++)
			if ((arr[j] < ((i+1)*delta)+min) &&
				arr[j] >= ((i)*delta)+min))
				N[i]++;
	
	/* vamos a calcular T */
	for(i = 0; i < k; i++)
		T += pow((N[i] - ((double) size * fun(arr[i]))), 2) * 
			(1.0/(double)size * fun(arr[i]));
	
	result = chi_cuadrada(gl, T);
	
	return result;
}


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
double get_p_value_ks(int k, double *arr, int size, double (*Fun)(double), int gl)
{
	/*! NOTE FIXME: */
}