#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "rdg.h"
#include "ssv.h"


/* # de intervalos en los que se divide el rango de la muestra */
#define  NI  10

/* # de simulaciones a ejecutar para ajustar el p-valor */
unsigned int NSIM = 10000;


/* Función de distribución acumulada de una v.a. ~ U(0,1) */
static double F (double x) { return x; }

int main (void)
{
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Intervalos */
	double I[NI] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
	/* Valores intermedios */
	double X[NI] = {0.05, 0.15, 0.25, 0.35, 0.45,
			0.55, 0.65, 0.75, 0.85, 0.95};
	/* Probabilidades teoricas de caer en los intervalos */
	double p[NI] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
	
	/* # de valores muestrales */
	unsigned int n = 10;
	/* Valores muestrales */
	double sample[NI] = {0.12, 0.18, 0.06, 0.33, 0.72,
		 	     0.83, 0.36, 0.27, 0.77, 0.74};
	/* Valores generados en una simulación */
	double *sim = NULL;
	
	/* Estadísticos de una muestra para Ji-2 y K-S resp. */
	double t = 0.0, d = 0.0;
	/* Estadísticos de cada simulación */
	double	Ti = 0.0, Di = 0.0;
	
	
	sim = (double *) malloc (n * sizeof (double));
	assert (sim != NULL);
	
	
	/** Test de Ji-2 */
	
	/* Metemos en 't' el estadístico de la muestra verdadera */
	t = ji_cuadrado (sample, n, I, NI, p);
	
	/* Simulamos */
	for (i=0 ; i<NSIM ; i++) {
		
		/* Simulamos una muestra según la distr. hip. 'p' */
		for (j=0 ; j<n ; j++)
			sim[j] = gen_prob (X, p, NI);
		
		/* Evaluamos su estadístico */
		Ti = ji_cuadrado (sim, n, I, NI, p);
		
		if (Ti > t)
			p_value += 1.0;
	}
	p_value = p_value / (double) NSIM;
	
	printf ("____________________________________________________________\n"
		"Test de Ji-cuadrado\n"
		"El estadístico de la muestra fue:\tt = %.8f\n"
		"Tras %u simulaciones se obtuvo:\tp-valor = %.8f\n",
		t, NSIM, p_value);
	
	
	/** Test de K-S */
	
	/* Metemos en 'd' el estadístico de la muestra verdadera */
	d = kolmogorov_smirnov (sample, n, F);
	
	/* Simulamos */
	p_value = 0.0;
	for (i=0 ; i<NSIM ; i++) {
		
		/* Simulamos una muestra según la distr. hip. 'p' */
		for (j=0 ; j<n ; j++)
			sim[j] = mzran13() / (double) ULONG_MAX;
		
		/* Evaluamos su estadístico */
		Di = kolmogorov_smirnov (sim, n, F);
		
		if (Di > d)
			p_value += 1.0;
	}
	p_value = p_value / (double) NSIM;
	
	printf ("____________________________________________________________\n"
		"Test de Kolmogorov-Smirnov\n"
		"El estadístico de la muestra fue:\td = %.8f\n"
		"Tras %u simulaciones se obtuvo:\tp-valor = %.8f\n",
		d, NSIM, p_value);

	free (sim);
	sim = NULL;
	
	return 0;
}
