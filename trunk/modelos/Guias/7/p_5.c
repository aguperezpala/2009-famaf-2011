#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include "rdg.h"
#include "ssv.h"


/* # de ensayos de la Binomial(n,p) */
#define  n  8

/* probabilidad de un éxito de la Binomial(n,p) */
double p = 0.0;

/* # de valores muestrales */
#define  N  18

/* # de intervalos en los que se divide el rango de la muestra */
#define  NI  n+1

/* # de simulaciones a ejecutar para ajustar el p-valor */
unsigned int NSIM = 10000;



/* Función de distribución acumulada de una Binomial(n,p) */
static double F (double x)
{
	unsigned int i = 0;
	double	c = p / (1.0-p),
		pi = pow (1.0-p, (double) n),	/* p0 = (1-p)^n */
		F = pi;				/* F(0) = p0 */
	
	for (i=1 ; (double)i <= x ; i++) {
		/* pi+1 = pi * (p/(1-p)) * ((n-i) / (i+1)) */
		pi = pi * c * (((double)(n-i)) / (double)(i+1));
		/* F(i+1) = F(i) + pi+1 */
		F += pi;
	}
	
	return F;
}



int main (void)
{
	double pSample = 0.0;
	double pSim    = 0.0;
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Intervalos */
	double I[NI] = {-0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5	};
	/* Valores intermedios */
	double X[NI] =  {  0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
	/* Probabilidades teoricas de caer en los intervalos */
	double probSample[NI];
	double probSim[NI];
	
	/* Valores muestrales */
	double sample[N] = {6.0, 7.0, 3.0, 4.0, 7.0, 3.0, 7.0, 2.0, 6.0,
			    3.0, 7.0, 8.0, 2.0, 1.0, 3.0, 5.0, 8.0, 7.0};
	/* Valores generados en una simulación */
	double sim[N];
	
	/* Estadísticos de una muestra para Ji-2 y K-S resp. */
	double t = 0.0, d = 0.0;
	/* Estadísticos de cada simulación */
	double	Ti = 0.0, Di = 0.0;
	
	
	/** Estimación de la distribución hipotética */
	
	/* Estimamos el parámetro 'p' con: sum(sample[i]) / (n*N) */
	pSample = 0.0;
	for (i=0 ; i<N ; i++)
		pSample += sample[i];
	pSample = pSample / (double) (n*N);
	p = pSample;
	
	printf ("Para la muestra dada se estimó un  p = %.8f\n", pSample);
	
	/* Calculamos las probabilidades P(X=x) con la Bin(n,p) estimada */
	prob_bin (n, pSample, probSample);
	
	
	/** Test de Ji-2 */
	
	/* Metemos en 't' el estadístico de la muestra verdadera */
	t = ji_cuadrado (sample, N, I, NI, probSample);
	
	/* Simulamos */
	for (i=0 ; i<NSIM ; i++) {
		
		/* Simulamos una muestra según la distr. estimada
		 * con los verdaderos valores muestrales */
		for (j=0 ; j<N ; j++)
			sim[j] = gen_prob (X, probSample, NI);
		
		/* Estimamos el parámetro 'p' con: sum(sim[i]) / (n*N) */
		pSim = 0.0;
		for (j=0 ; j<N ; j++)
			pSim += sim[j];
		pSim = pSim / (double) (n*N);
		
/*		printf ("#%u pSim = %.4f\n", i, pSim);
*/		
		/* Recalculamos las probabilidades con este 'p' simulado */
		prob_bin (n, pSim, probSim);
		
		/* Evaluamos su estadístico */
		Ti = ji_cuadrado (sim, N, I, NI, probSim);
		
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
	
	/* El parámetro 'p' de la distribución hip. F en principio es pSample */
	p = pSample;
	
	/* Metemos en 'd' el estadístico de la muestra verdadera */
	d = kolmogorov_smirnov (sample, N, F);
	
	/* Simulamos */
	p_value = 0.0;
	for (i=0 ; i<NSIM ; i++) {
		
		/* Simulamos una muestra usando la distr Bin(n,p)
		 * que usa el parámetro 'p' estimado con la muestra */
		for (j=0 ; j<N ; j++)
			sim[j] = gen_bin (n, pSample);
			
		/* Estimamos el parámetro 'p' con: sum(sim[i]) / (n*N) */
		p = 0.0;
		for (j=0 ; j<N ; j++)
			p += sim[j];
		p = p / (double) (n*N);
		
		/* Evaluamos el estadístico de esta simulación
		 * Notar que ahora F(x) utiliza el 'p' estimado con la sim. */
		Di = kolmogorov_smirnov (sim, N, F);
		
		if (Di > d)
			p_value += 1.0;
	}
	p_value = p_value / (double) NSIM;
	
	printf ("____________________________________________________________\n"
		"Test de Kolmogorov-Smirnov\n"
		"El estadístico de la muestra fue:\td = %.8f\n"
		"Tras %u simulaciones se obtuvo:\tp-valor = %.8f\n",
		d, NSIM, p_value);
	
	return 0;
}
