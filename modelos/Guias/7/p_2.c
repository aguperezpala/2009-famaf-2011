#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "rdg.h"
#include "ssv.h"


/* # de intervalos en los que se divide el rango de la muestra */
#define  NI  6

#define  SEXTO  0.1666666666

/* # de simulaciones a ejecutar para ajustar el p-valor */
unsigned int NSIM = 10000;


int main (void)
{
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Intervalos */
	double I[NI] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
	/* Valores intermedios */
	double X[NI] = {1.5, 2.5, 3.5, 4.5, 5.5, 6.5};
	/* Probabilidades teoricas de caer en los intervalos */
	double p[NI] = {SEXTO, SEXTO, SEXTO, SEXTO, SEXTO, SEXTO};
	
	/* # de valores muestrales */
	unsigned int n = 1000;
	/* Frecuencias muestrales */
	double N[NI] = {158.0, 172.0, 164.0, 181.0, 160.0, 165.0};
	
	/* Estadístico de la muestra */
	double t = 0.0;
	/* Valores generados en una simulación */
	double *sim = NULL;
	/* Estadístico de cada simulación */
	double Ti = 0.0;
	
	
	/* Metemos en 't' el estadístico de la muestra verdadera */
	for (i=0 ; i<NI ; i++)
		t += pow (N[i] - (double) n * p[i], 2.0) / ((double) n * p[i]);
	
	
	sim = (double *) malloc (n * sizeof (double));
	assert (sim != NULL);
	
	
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
	
	printf ("El estadístico de la muestra fue:\tt = %.8f\n"
		"Tras %u simulaciones se obtuvo:\tp-valor = %.8f\n",
		t, NSIM, p_value);
	
	free (sim);
	sim = NULL;
	
	return 0;
}
