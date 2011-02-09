#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "analisis_estadistico.h"
#include "gen_discretas.h"

/*! Mal hecho mirar el de xcarlox mejor */

/* # de intervalos en los que se divide el rango de la muestra */
#define  NI  8

/* # de simulaciones a ejecutar para ajustar el p-valor */
#define NSIM  10000

static void generar_prob_bin (unsigned int n, double p, double *prob)
{
	unsigned int i = 0;
	double	c = p / (1.0-p),
		pi = pow (1.0-p, (double) n);
	
	assert (prob != NULL);
	
	do {
		prob[i] = pi;
		pi = pi * (((double)(n-i)) / (double)(i+1)) * c;
		i++;
	} while (i <= n);
	
	return;
}


int main5 (void)
{
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Intervalos */
	double I[NI] = {0.0, 1.0, 2.0,3,4,5,6,7};
	/* Probabilidades teoricas de caer en los intervalos */
	double p[NI];
	
	/* # de valores muestrales */
	unsigned int n = 18;
	/* Frecuencias muestrales */
	double N[NI] = {1,2,4,1,1,2,5,1};
	
	/* Estadístico de la muestra */
	double t = 0.0;
	/* Valores generados en una simulación */
	double *sim = NULL;
	/* Estadístico de cada simulación */
	double Ti = 0.0;
	
	double Mu = 0.0;
	double Sample[18] = {6,7,3,4,7,3,7,2,6,3,7,8,2,1,3,5,8,7};
	/* debemos estimar mu ya que no lo conocemos con los datos de la muestra */
	Mu = estimar_media_muest_inef(Sample, n);
	
	/* calculamos las probabilidades de una bin hasta de parametro n */
	generar_prob_bin (NI, Mu, p);
	
	/* Metemos en 't' el estadístico de la muestra verdadera */
	for (i=0 ; i<NI ; i++)
		t += pow (N[i] - (double) n * p[i], 2.0) / ((double) n * p[i]);
	
	
	sim = (double *) malloc (n * sizeof (double));
	assert (sim != NULL);
	
	
	for (i=0 ; i<NSIM ; i++) {
		
		generar_prob_bin(n, Mu, sim);
		
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
