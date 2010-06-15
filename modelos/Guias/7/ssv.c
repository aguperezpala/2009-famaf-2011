#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include "rdg.h"
#include "ssv.h"

#define SIZE 2
#define MONTECARLO_BOUND 10000

#define PAD 40	/* Definido para arquitectura de 64 bits */

/** Estructura Media muestral */
double	media[SIZE] = {0.0, 0.0};
unsigned int	ma = 0,  /* media[ma] = media anterior  = X(n-1) */
		ms = 0;  /* media[ms] = media siguiente = X(n)   */

/** Estructura Varianza muestral */
double	var[SIZE] = {0.0, 0.0};		/* Varianza muestral */
unsigned int	va = 0,  /* var[va] = varianza anterior  = S^2(n-1) */
		vs = 0;  /* var[vs] = varianza siguiente = S^2(n)   */



/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la media muestral = X(n)
 */
double media_m (double Xn, double n)
{
	/* Recordar:	media[ma] = media anterior  = X(n-1) *
	 *		media[ms] = media siguiente = X(n)   *
	 *						     */
	ma = ms;
	ms = (ms+1) % SIZE;
	media[ms] = media[ma] + (Xn-media[ma]) / n;
	
	return media[ms];
}


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la varianza muestral = S^2(n)
 *
 * NOTE: ASUME que (media[ma] , media[ms]) tienen los valores (X(n-1) , X(n))
 * 	 respectivamente (ver arriba la funcion media_m, "Media muestral")
 */
double var_m (double Xn, double n)
{
	/* Recordar:	var[va] = varianza anterior  = S^2(n-1) *
	 *		var[vs] = varianza siguiente = S^2(n)	*
	 *							*/
	if (n > 1) {
		va = vs;
		vs = (vs+1) % SIZE;
		var[vs] = ((n - 1.0) / n) * var[va] + \
			(n + 1.0) * pow (media[ms] - media[ma], 2.0);
	}
	
	return var[vs];
}


/* Reinicializa la media, como si volviesemos al paso 0 */
void reset_media_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		media[i] = 0.0;
	return;
}


/* Reinicializa la varianza, como si volviesemos al paso 0 */
void reset_var_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		var[i] = 0.0;
	return;
}



/* Método de Bootstrap para estimar el Error Cuadrático Medio (ECM)
 * del estimador "Media muestral" con respecto a la verdadera media µ,
 * dada una muestra de 'n' datos pasada como primer argumento
 *
 * Utiliza Montecarlo para devolver un valor que aproxima al ECM exacto
 *
 * PRE: sample != NULL
 */
double bootstrap_media (double *sample, unsigned int n)
{
	double ecm = 0.0;
	unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Xc = 0.0;	/* Media muestral de una configuración */
	int *config = NULL;	/* Configuración posible dada la muestra */
	
	long idum = 0;
	struct timeval tv;
	
	assert (sample != NULL);
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	/* Metemos la media empírica de la muestra real en Xe */
	for (i=0 ; i<n ; i++)
		Xe += sample[i] / (double) n;
	
	
	/* Vamos a aproximar el ECM empleando Montecarlo */
	ecm = 0.0;
	for (i=0 ; i<MONTECARLO_BOUND ; i++) {
		
		/* Guardamos en Xc la media muestral de una
			* configuracion aleatoria de valores de la muestra */
		reset_media_m ();
		for (j=0 ; j<n ; j++) {
			k = ran2(&idum) * n;
/*				k = mzran13() % n;
*/				Xc = media_m (sample[k], j+1);
		}
		
		/* Actualizamos el valor del ECM */
		ecm += (pow (Xc - Xe, 2.0)) / (double) MONTECARLO_BOUND;
		
		if (!(i%500))
			printf ("loop # %u\tecm == %.4f\n", i, ecm);
	}
	
	return ecm;
}
