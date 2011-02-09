#ifndef _GNU_SOURCE
  #define _GNU_SOURCE
#endif
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include "../file_reader.h"
#include "../rdg.h"
#include "../ssv.h"
#include "../distrib.h"
#include "../actividad4.h"


/* Nombre del archivo donde estan los valores muestrales */
#define  SAMPLE_FILE	"results/sample.dat"

/* # de valores muestrales */
#define  SAMPLE_SIZE	500

/* # de intervalos en los que se particiona la muestra */
#define  NI	10

/* Extremo izquierdo del primer intervalo */
#define  LI	0.0

/* Extremo derecho del mayor intervalo */
#define  UI	1.5

/* # de simulaciones a realizar para ajustar el p-valor */
#define  NUM_SIM  10000

/* Freedom Degree's for the Gamma function Ji-2 aprox distribution */
#define  FRDG	NI-1

/* Freedom Degree's for the Normal function Ji-2 aprox distribution */
#define  FRDN	NI-1

/* Freedom Degree's for the LogNormal function Ji-2 aprox distribution */
#define  FRDLN	NI-1



/** ### UNIFORME [0..1] */
static double Uniform (double x) { return x; }



/** ### GAMMA */
/* Funcion de densidad de una Gamma(α,β) con α ~ 8.95 y β ~ 0.0569102
static double Gamma (double x)
{
	assert (x > 0.0);
	
	return 3819000.0 * pow (M_E, -17.5716 * x) * pow (x, 7.95);
	
}*/

/* Funcion acumulada de una Gamma(α,β) con α ~ 8.95 y β ~ 0.0569102 */
static double F_Gamma (double x)
{
	static double a = 8.95, b = 0.0569102;
	return gammp (a, x/b);
}



/** ### NORMAL */
/* Funcion de densidad de una Normal(μ,σ) con μ ~ 0.50934599 y σ ~ 0.17069248
static double Normal (double x)
{
	return 2.3372 * exp(-17.1609 * pow (x-0.509346, 2.0));
}*/

/* Funcion acumulada de una Normal(μ,σ) con μ ~ 0.50934599 y σ ~ 0.17069248 */
static double F_Normal (double x)
{
	return 0.5 * (erf (4.14258*(x-0.509346)) + 1.0);
}



/** ### LOG-NORMAL */
/* Funcion densidad de una LogNormal(μ,σ) con μ ~ -0.7312493 y σ ~ 0.3418444
static double LogNormal (double x)
{
	return 1.16718 * exp (-4.27982 * pow (log(x) + 0.7312, 2.0)) / x;
}*/

/* Funcion acumulada de una LogNormal(μ,σ) con μ ~ -0.7312493 y σ ~ 0.3418444 */
static double F_LogNormal (double x)
{
	return 0.5 * (erf (2.0685 * (log (x) + 0.731249)) + 1.0);
}




/* Guarda los valores probabilisticos listados en 'prob', que son 'n' en total,
 * dentro de un archivo de nombre 'fname'
 * Imprime el mensaje 'msg' al comienzo del archivo (sii 'msg' != NULL)
 *
 * PRE: fname != NULL
 *	prob  != NULL && #(prob) == n
 */
static void save_probabilities (const char* fname, double *prob,
				unsigned int n, const char *msg)
{
	FILE *f = NULL;
	unsigned int i = 0;
	
	assert (fname != NULL);
	assert (prob  != NULL);
	
	f = fopen (fname,"w");
	assert (f != NULL);
	
	if (msg != NULL)
		fprintf (f, "%s\n", msg);
	
	for (i=0 ; i<n ; i++)
		fprintf (f, "%u) %.8f\n", i, (double) SAMPLE_SIZE * prob[i]);
	
	fclose (f);
	
	return;
}





/** ### MAIN */
int main (void)
{
	double p_value = 0.0;
	
	double I[NI];		/* Intervalos */
	double Xvals[NI];	/* Valores internos a los intervalos */
	/* Probabilidades teoricas de caer en los intervalos */
	double pGamma[NI];
	double pNormal[NI];
	double pLogNormal[NI];
	
	double sample[SAMPLE_SIZE];	/* Valores muestrales */
	double sim[SAMPLE_SIZE];	/* Valores generados en una simulación */
	
	/* Estadísticos de Ji-2 */
	double	Tg  = 0.0,	/* Gamma */
		Tn  = 0.0,	/* Normal */
		Tln = 0.0;	/* LogNormal */
		
	/* Estadísticos de K-S */
	double	Dg  = 0.0,	/* Gamma */
		Dn  = 0.0,	/* Normal */
		Dln = 0.0;	/* LogNormal */
	
	/* Estadísticos de Ji-2 y K-S para una simulacion */
	double Ti = 0.0, Di = 0.0;
	unsigned int i = 0, j = 0;
	
	/* Introducimos los valores muestrales en el arreglo "sample" */
	read_double_file (sample, SAMPLE_SIZE, SAMPLE_FILE);
	
	/* Generamos el arreglo de intervalos de division de los datos */
	act4_gen_intervals (LI, UI, NI, I);
	
	/* Creamos el arreglo de valores internos a los intervalos, para simular */
	act4_gen_values (I, NI, Xvals);

	
	/** Calculamos los Estadísticos de las distribuciones según Ji-2 */
	
	printf ("Estadísticos según Ji-2:\n");
	
	/* Prob. de caer en cada intervalo según Gamma(α,β) */
	act4_gen_pi (pGamma, I, NI, F_Gamma);
	Tg = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, pGamma);
	
	printf ("\tGamma T = %.8f\n", Tg),
	save_probabilities ("Gamma_interval_probs.dat", pGamma, NI,
			    "Probabilities for defined intervals according to "
			    "the Gamma distribution\n");
	
	
	/* Prob. de caer en cada intervalo según Normal(μ,σ) */
	act4_gen_pi (pNormal, I, NI, F_Normal);
	Tn = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, pNormal);
	
	printf ("\tNorm T = %.8f\n", Tn);
	save_probabilities ("Normal_interval_probs.dat", pNormal, NI,
			    "Probabilities for defined intervals according to "
			    "the Normal distribution\n");
	
	
	/* Prob. de caer en cada intervalo según LogNormal(μ,σ) */
	act4_gen_pi (pLogNormal, I, NI, F_LogNormal);
	Tln = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, pLogNormal);
	
	printf ("\tLogNorm T = %.8f\n", Tln);
	save_probabilities ("LogNormal_interval_probs.dat", pLogNormal, NI,
			    "Probabilities for defined intervals according to "
			    "the LogNormal distribution\n");
	
	
	/** p-valores según Ji-2 */
	
	printf ("\nValores-p según Ji-2 (con aproximación)\n");
	
	/* Gamma */
	p_value = chi_cuadrada (FRDG, Tg);
	printf ("\tGamma p-val = %.8f\n", p_value);
	
	/* Normal */
	p_value = chi_cuadrada (FRDN, Tn);
	printf ("\tNorm p-val = %.8f\n", p_value);
	
	/* LogNormal */
	p_value = chi_cuadrada (FRDLN, Tln);
	printf ("\tLogNorm p-val = %.8f\n", p_value);
	
	
	printf ("\nValores-p según Ji-2 (con simulación)\n");
	
	/* Gamma */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = gen_prob (Xvals, pGamma, NI);
		
		Ti = ji_cuadrado (sim, SAMPLE_SIZE, I, NI, pGamma);
		
		if (Ti > Tg)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tGamma p-val = %.8f\n", p_value);
	
	/* Normal */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = gen_prob (Xvals, pNormal, NI);
		
		Ti = ji_cuadrado (sim, SAMPLE_SIZE, I, NI, pNormal);
		
		if (Ti > Tn)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tNormal p-val = %.8f\n", p_value);
	
	/* LogNormal */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = gen_prob (Xvals, pLogNormal, NI);
		
		Ti = ji_cuadrado (sim, SAMPLE_SIZE, I, NI, pLogNormal);
		
		if (Ti > Tln)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tLogNormal p-val = %.8f\n", p_value);
	
	
	
	/** Calculamos los Estadísticos de las distribuciones según K-S */
	
	printf ("\n\nEstadísticos según Kolmogorov-Smirnov:\n");
	
	Dg = kolmogorov_smirnov (sample, SAMPLE_SIZE, F_Gamma);
	printf ("\tGamma D = %.8f\n", Dg);
	
	Dn = kolmogorov_smirnov (sample, SAMPLE_SIZE, F_Normal);
	printf ("\tNorm D = %.8f\n", Dn);
	
	Dln = kolmogorov_smirnov (sample, SAMPLE_SIZE, F_LogNormal);
	printf ("\tLogNorm D = %.8f\n", Dln);
	
	printf ("\nValores-p según K-S:\n");
	
	
	/** p-valores segun K-S */
	
	/* Gamma */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = mzran13() / (double) ULONG_MAX;
		
		Di = kolmogorov_smirnov (sim, SAMPLE_SIZE, Uniform);
		
		if (Di > Dg)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tGamma p-val = %.8f\n", p_value);
	
	/* Normal */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = mzran13() / (double) ULONG_MAX;
		
		Di = kolmogorov_smirnov (sim, SAMPLE_SIZE, Uniform);
		
		if (Di > Dn)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tNorm p-val = %.8f\n", p_value);
	
	/* LogNormal */
	p_value = 0.0;
	for (i=0 ; i<NUM_SIM ; i++) {
		
		for (j=0 ; j < SAMPLE_SIZE ; j++)
			sim[j] = mzran13() / (double) ULONG_MAX;
		
		Di = kolmogorov_smirnov (sim, SAMPLE_SIZE, Uniform);
		
		if (Di > Dln)
			p_value += 1.0;
	}
	p_value = p_value / (double) NUM_SIM;
	printf ("\tLogNorm p-val = %.8f\n", p_value);
	
	return 0;
}
