#include "analisis_estadistico.h"


/******************************************************************************/
double estimar_media_muestral(int j, double Xpromj, double Xj1)
{
	return (Xpromj + (Xj1 - Xpromj)/(j+1));
}

/******************************************************************************/
double estimar_varianza_muestral(int j, double Sj, double Xpromj1, double Xpromj)
{
	return (((j - 1.0)/(double)j)*Sj + ((double)(j+1.0))*pow(Xpromj1-Xpromj,2));
}

/******************************************************************************/
double estimar_media_muest_inef(double *sample, int N)
{
	int i = 0;
	double result = 0;
	
	for(i; i < N; i++)
		result += sample[i];
	
	return result/(double)N;
}

/******************************************************************************/
double estimar_var_muest_inef(double *sample, int N, double mediaMuestral)
{
	int i = 0;
	double result = 0;
	
	for(i; i < N; i++)
		result += pow(sample[i]-mediaMuestral,2);
	
	return result/(double)N;
}


/******************************************************************************/

/******************************************************************************/

/*!*****************************************************************************/

#define SIZE	2
#define MONTECARLO_BOUND 10000		/* iteraciones de montecarlo */


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
static double media_m (double Xn, double n)
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
static double var_m (double Xn, double n)
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
static void reset_media_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		media[i] = 0.0;
	return;
}


/* Reinicializa la varianza, como si volviesemos al paso 0 */
static void reset_var_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		var[i] = 0.0;
	return;
}


double bootstrap_media (double *sample, unsigned int n)
{
	double ecm = 0.0;
	unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;
	unsigned long N = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Xc = 0.0;	/* Media muestral de una configuración */
	int *config = NULL;	/* Configuración posible dada la muestra */
	
	
	/* Metemos la media empírica de la muestra real en Xe */
	for (i=0 ; i<n ; i++)
		Xe += sample[i] / (double) n;
	
	N = pow ((double) n, (double) n);
	ecm = 0.0;
	
	/* Aca definimos tolerancia para los datos en 6 valores, si son menos de
	 * 6 calculamos todas las posibles configuraciones (pow(5,5))*/
	if (n < 6) {
		/* Vamos a calcular las cosas de manera exacta,
		* considerando todas las configuraciones posibles */
		config = (int *) calloc (n , sizeof(int));
		
		for (i=0 ; i<N ; i++) {
			
			/* Determinamos cual es esta configuración */
			row = i / n;
			col = i % n;
			config[row] += col;
			
			/* Guardamos su media muestral en Xc */
			reset_media_m ();
			for (j=0 ; j<n ; j++)
				Xc  = media_m (sample[config[j]], j+1);
				
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Xc - Xe, 2.0)) / (double) N;
		}
		free (config);
		config = NULL;
		
	} else {
		/* Vamos a aproximar el ECM empleando Montecarlo */
		
		for (i=0 ; i<MONTECARLO_BOUND ; i++) {
			
			/* Guardamos en Xc la media muestral de una
			* configuracion aleatoria de valores de la muestra */
			reset_media_m ();
			for (j=0 ; j<n ; j++) {
				k = disc_gen_uniforme(0, n);
				Xc = media_m (sample[k], j+1);
			}
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Xc - Xe, 2.0)) / (double) MONTECARLO_BOUND;
			/*
			if (!(i%500))
				printf ("loop # %u\tecm == %.4f\n", i, ecm);*/
		}
	}
	
	return ecm;
}

/******************************************************************************/
double bootstrap_varianza (double *sample, unsigned int n)
{
	double ecm = 0.0;
	unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;
	unsigned long N = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Ve = 0.0;	/* Varianza empirica */
	double Vc = 0.0;	/* Varianza muestral de una configuración */
	int *config = NULL;	/* Configuración posible dada la muestra */
	double aux = 0;
	
	/* Metemos la media empírica de la muestra real en Xe */
	for (i=0 ; i<n ; i++)
		Xe += sample[i] / (double) n;
	
	/* calculamos la varianza empirica = Sumatoria pow((Xi-Xe),2)/n fomr 1 to n */
	for(i = 0; i < n; i++) {
		Ve += pow(sample[i]-Xe,2)/(double)n;
	}
	
	N = pow ((double) n, (double) n);
	ecm = 0.0;
	
	/* Aca definimos tolerancia para los datos en 6 valores, si son menos de
	* 6 calculamos todas las posibles configuraciones (pow(5,5))*/
	if (n < 1) {
		/* Vamos a calcular las cosas de manera exacta,
		* considerando todas las configuraciones posibles */
		config = (int *) calloc (n , sizeof(int));
		
		for (i=0 ; i<N ; i++) {
			
			/* Determinamos cual es esta configuración */
			row = i / n;
			col = i % n;
			config[row] += col;
			
			/* Guardamos su varianza muestral en Vc */
			reset_var_m ();
			reset_media_m();
			for (j=0 ; j<n ; j++){
				aux = media_m (sample[config[j]], j+1);
				Vc  = var_m(sample[config[j]], j+1);
				printf("Seleccionada X[%d]\t", config[j]);
				/*printf("Media: %G\tVarianza: %G\n", aux, Vc);*/
			}
			printf("\n");
			/*printf("Media: %G\n", media[ms]);
			printf("Varianza: %G\n\n\n", Vc);*/
			/* Actualizamos el valor del ECM */
			ecm += (pow (Vc - Ve, 2.0)) / (double) N;
		}
		free (config);
		config = NULL;
		
	} else {
		/* Vamos a aproximar el ECM empleando Montecarlo */
		
		for (i=0 ; i<MONTECARLO_BOUND ; i++) {
			
			/* Guardamos en Xc la media muestral de una
			* configuracion aleatoria de valores de la muestra */
			reset_media_m ();
			reset_var_m ();
			for (j=0 ; j<n ; j++) {
				k = disc_gen_uniforme(0, n);
				media_m (sample[k], j+1);
				Vc  = var_m (sample[k], j+1);
			}
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Vc - Ve, 2.0)) / (double) MONTECARLO_BOUND;
			/*
			if (!(i%500))
			printf ("loop # %u\tecm == %.4f\n", i, ecm);*/
		}
	}
	
	return ecm;
}
/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/








