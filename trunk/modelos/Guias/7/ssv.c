#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <sys/time.h>
#include "rdg.h"
#include "ssv.h"

#define SIZE 2
#define MONTECARLO_BOUND 10000

#define PAD 40	/* Definido para arquitectura de 64 bits */

#define  MAX(x,y)  ( ((x) > (y)) ? (x) : (y) )


/* Función para operación verborrágica */
#ifdef _VERBOSE
  #define show(s,...) printf(s, ##__VA_ARGS__)
#else
  #define show(s,...)
#endif



/** Estructura Media muestral */
double	media[SIZE] = {0.0, 0.0};
unsigned int	ma = 0,  /* media[ma] = media anterior  = X(n-1) */
		ms = 0;  /* media[ms] = media siguiente = X(n)   */

/** Estructura Varianza muestral */
double	var[SIZE] = {0.0, 0.0};		/* Varianza muestral */
unsigned int	va = 0,  /* var[va] = varianza anterior  = S^2(n-1) */
		vs = 0;  /* var[vs] = varianza siguiente = S^2(n)   */




/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~  MEDIA  -  VARIANZA  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


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




/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~  BOOTSTRAP --> ERROR CUADRATICO MEDIO  ~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


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
	unsigned int i = 0, j = 0, k = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Xc = 0.0;	/* Media muestral de una configuración */
	
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




/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~  SIMULACION  -  ESTADISTICOS  ~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */



/* Vuelve con el # de intervalo en el que cae el valor Xi
 *
 * PRE: I != NULL
 *	los valores en I están en orden estrictamente creciente
 *
 * ie:	Xj == 0.25		 |
 *	 I == { [0.00 , 0.15),	 |
 *		[0.15 , 0.55),	 |>  find_interval (Xj, I, 4) == 1
 *		[0.55 , 1.12),	 |
 *		[1.12 , 2.00) }  |
 */
static unsigned int find_interval (double Xj, double *I, unsigned int k)
{
	unsigned int i = 0;
	
	assert (I != NULL);
	
	while (i<k-1 && Xj > I[i+1]-DBL_EPSILON)
		i++;
	
	return i;
}


/* Estadístico del test Ji-cuadrado para una muestra 'sample' de 'n' valores
 * Los intervalos de agrupacion de resultados deben estar en el parametro 'I'
 * p[i] == "probabilidad de caer en el intervalo Int(i)"
 *
 * Se define al i-esimo intervalo Int(i) como:
 *	Int(i) = [ I[i] , I[i+1] )
 * y para el ultimo intervalo vale que:
 *	Int(k) = [ I[k] , inifinity )
 *
 * PRE: sample != NULL	&&  n == #(sample)
 *	I != NULL	&&  k == #(I)
 *	p != NULL	&&  k == #(p)
 */
double ji_cuadrado (double *sample, unsigned int n,
		    double *I, unsigned int k, double *p)
{
	double t = 0.0;
	unsigned int i = 0, j = 0;
	unsigned int *N = NULL;
	
	assert (sample != NULL);
	assert (I!= NULL);
	
	N = (unsigned int *) calloc (k, sizeof (unsigned int));
	assert (N != NULL);
	
	for (j=0 ; j<n ; j++) {
		i = find_interval (sample[j], I, k);
		N[i]++;
	}
	
	for (i=0 ; i<k ; i++)
		t += pow (N[i] - (double) n * p[i], 2.0) / ((double) n * p[i]);
	
	return t;
}




/* Compara dos 'doubles' pasados por referencia y devuelve:
 * a > b  =>  1
 * a = b  =>  0
 * a < b  => -1
 */
static int cmp_dbl (const void *a, const void *b)
{
	return  (*((const double *) a) > *((const double *) b)) ?  1 : \
		(*((const double *) a) < *((const double *) b)  ? -1 : 0);
}


/* Estadístico del test Kolmogorov-Smirnov de una muestra 'sample' con 'n' datos
 * 'F' es la función de probabilidad teórica a aplicar sobre los datos
 *
 * PRE: sample != NULL  &&  n == #(sample)
 *	F != NULL
 */
 double kolmogorov_smirnov (double *sample, unsigned int n, double (*F) (double))
{
	double	d = -DBL_MAX,
		j = 0.0,
		m = (double) n,
		Fj = 0.0;
	
	assert (sample != NULL);
	assert (F != NULL);
	
	/* Ordenamos la muestra en orden creciente */
	qsort (sample, (size_t) n, sizeof (double), cmp_dbl);
	
	/* Buscamos el estadístico */
	for (j = 0.0 ; j < m ; j += 1.0) {
		
		Fj = F (sample[(int)j]);
		
		/* Notar que aquí 0 <= j < n, por lo cual usamos j+1 */
		d = MAX (d , (j+1.0)/m - Fj );
		d = MAX (d , Fj - j/m);
		
		show ("D%.0f+ = %.4f\tD%.0f- = %.4f\n",
			j, (j+1.0)/m - Fj, j, Fj - j/m);
	}
	
	show ("Estadístico K-S:  D = %.4f\n", d);
	
	return d;
}
