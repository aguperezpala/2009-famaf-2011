#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "rdg.h"
#include "ssv.h"


/* # de valores muestrales */
#define  N  13

/* # de simulaciones a ejecutar para ajustar el p-valor */
unsigned int NSIM = 10000;

/* Parámetro de la exponencial propuesta por H0 */
double lambda = 1.0/50.0;



/* Función de distribución acumulada de una v.a. ~ U(0,1) */
static double U (double x) { return x; }

/* Función de distribución acumulada de una v.a. ~ Exp(lambda) */
static double F (double x) { return 1.0 - pow (M_E, -lambda * x); }


int main (void)
{
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Valores muestrales */
	double sample[N] = {8.0, 11.0, 22.0, 33.0, 41.0, 75.0, 78.0,
			    86.0, 99.0, 122.0, 133.0, 144.0, 146.0};
	/* Valores generados en una simulación */
	double *sim = NULL;
	
	/* Estadístico de la muestra para K-S */
	double d = 0.0;
	/* Estadísticos de cada simulación */
	double Di = 0.0;
	
	sim = (double *) malloc (N * sizeof (double));
	assert (sim != NULL);
	
	/** Test de K-S */
	
	/* Metemos en 'd' el estadístico de la muestra verdadera */
	d = kolmogorov_smirnov (sample, N, F);
	
	/* Simulamos */
	p_value = 0.0;
	for (i=0 ; i<NSIM ; i++) {
		
		/* Simulamos una muestra según la distr. hip. 'p' */
		for (j=0 ; j<N ; j++)
			sim[j] = mzran13() / (double) ULONG_MAX;
		
		/* Evaluamos su estadístico */
		Di = kolmogorov_smirnov (sim, N, U);
		
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
