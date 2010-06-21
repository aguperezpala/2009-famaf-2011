#include "actividad3.h"

/* Funcion que imprime los parametros de la normal.
* REQUIRES:
* 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
*	size	(tamaño de datos a leer)
* RETURNS:
* 	< 0	on error
* 	0	if success
*/
int act3_normal_params(const char *fname, int size)
{
	double data[size];
	int result = -1;
	double var = 0;
	
	
	assert(fname != NULL);
	
	/* leemos los datos */
	result = read_double_file(data, size, fname);
	if (result < 0)
		return result;
	
	assert(result == size);
	
	var = act2_get_varianza(data, size);
	var = ((double)(size-1)/(double)size) * var;
	var = sqrt(var);
	
	/*	Funcion de densidad de la Normal para los parametros estimados:
	2.3372 * exp(-17.1609 * sqr(x-0.509346))
	
	*/	
	printf("Normal params:\nµ:\t%.8f\t\tσ:\t%.8f\n",act2_get_media(data,size),
		var);
	
	return 0;
}

/* Funcion que imprime los parametros de la log-normal.
* REQUIRES:
* 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
*	size	(tamaño de datos a leer)
* RETURNS:
* 	< 0	on error
* 	0	if success
*/
int act3_lnNormal_params(const char *fname, int size)
{
	double data[size];
	int result = -1, i = 0;
	double var = 0, mu = 0;
	
	
	assert(fname != NULL);
	
	/* leemos los datos */
	result = read_double_file(data, size, fname);
	if (result < 0)
		return result;
	
	assert(result == size);
	
	for(i = 0; i < size; i++)
		mu += log(data[i]);
	mu = mu / (double) size;
	
	for(i = 0; i < size; i++)
		var += pow(log(data[i]) - mu, 2)/(double)size;
	var = sqrt(var);
	
	
	printf("LogNormal params:\nµ:\t%.8f\t\tσ:\t%.8f\n", mu, var);
	
/*	Funcion de densidad de la LogNormal para los parametros estimados:
	(1.16718 * exp (-4.27982 * sqr ( ln(x) + 0.7312))) / x
*/	
	return 0;
}



/* Funcion que imprime los parametros de la gamma.
* REQUIRES:
* 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
*	size	(tamaño de datos a leer)
* RETURNS:
* 	< 0	on error
* 	0	if success
*/
int act3_gamma_params(const char *fname, int size)
{
	double data[size];
	int result = -1, i = 0;
	double T = 0, mu = 0;
	
	
	assert(fname != NULL);
	
	/* leemos los datos */
	result = read_double_file(data, size, fname);
	if (result < 0)
		return result;
	
	assert(result == size);
	
	mu = act2_get_media(data,size);
	
	for(i = 0; i < size; i++)
		T += log(data[i])/(double)size;
	
	T = log(mu) - T;
	T = pow(T, -1);
	/* funcion de densidad para la funcion gamma con parametros
	* alfa = 8.913		beta = 0.057145742
	x^(( 8.913)-1) * exp(-x/(0.0571457)) * 119951465170,848193661/33483.67682051149
	*/
	printf("Gamma params:\nT:\t%.8f\t\tX(n):\t%.8f\n", T, mu);
	
	return 0;
}

