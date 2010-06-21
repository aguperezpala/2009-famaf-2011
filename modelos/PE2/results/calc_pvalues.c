#include <stdio.h>
#include <assert.h>
#include <math.h>
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
#define  NI	25

/* Extremo izquierdo del primer intervalo */
#define  LI	0.0

/* Extremo derecho del mayor intervalo */
#define  UI	1.5

/* Freedom Degree's for the Gamma function Ji-2 aprox distribution */
#define  FRDG	NI-2

/* Freedom Degree's for the Normal function Ji-2 aprox distribution */
#define  FRDN	NI-2

/* Freedom Degree's for the LogNormal function Ji-2 aprox distribution */
#define  FRDLN	NI-2


/** ### GAMMA */
/* Funcion de densidad de una Gamma(α,β) con α ~ 8.913 y β ~ 0.0571457 */
static double Gamma (double x)
{
	static double	a = 8.913,	/* α */
			b = 0.0571457,	/* β */
			c = 119951465170.848193661/33483.67682051149;
	
	assert (x > 0.0);
	
	return pow (x, a-1.0) * exp (-x/b) * c;
}

/* Funcion acumulada de una Gamma (estimada con Montecarlo) */
static double F_Gamma (double x)
{
	return act4_monte_carlo ((double) LI, x, Gamma);
}



/** ### NORMAL */
/* Funcion de densidad de una Normal(μ,σ) con μ ~ 0.50934599 y σ ~ 0.17069248 */
static double Normal (double x)
{
	return 2.3372 * exp(-17.1609 * pow (x-0.509346, 2.0));
}

/* Funcion acumulada de una Normal (estimada con Montecarlo) */
static double F_Normal (double x)
{
	return act4_monte_carlo ((double) LI, x, Normal);
}



/** ### LOG-NORMAL */
/* Funcion densidad de una LogNormal(μ,σ) con μ ~ -0.7312493 y σ ~ 0.3418444 */
static double LogNormal (double x)
{
	return 1.16718 * exp (-4.27982 * pow (log(x) + 0.7312, 2.0)) / x;
}

/* Funcion acumulada de una LogNormal (estimada con Montecarlo) */
static double F_LogNormal (double x)
{
	return act4_monte_carlo ((double) LI, x, LogNormal);
}




/** ### MAIN */
int main (void)
{
	double p_value = 0.0;
	
	double I[NI];	/* Intervalos */
	double p[NI];	/* Probabilidades teoricas de caer en los intervalos */
	
	double sample[SAMPLE_SIZE];/* Valores muestrales */
	double sim[NI];		/* Valores generados en una simulación */
	
	/* Estadisticos de Ji-2 */
	double	Tg  = 0.0,	/* Gamma */
		Tn  = 0.0,	/* Normal */
		Tln = 0.0;	/* LogNormal */
		
	/* Estadisticos de K-S */
	double	Dg  = 0.0,	/* Gamma */
		Dn  = 0.0,	/* Normal */
		Dln = 0.0;	/* LogNormal */
	
	
	/* Introducimos los valores muestrales en el arreglo "sample" */
	read_double_file (sample, SAMPLE_SIZE, SAMPLE_FILE);
	
	/* Generamos el arreglo de intervalos de division de los datos */
	act4_gen_intervals (LI, UI, NI, I);
	
	
	/** Calculamos los estadisticos de las distribuciones segun Ji-2 */
	
	/* Prob. de caer en cada intervalo segun Gamma(α,β) */
	act4_gen_pi (p, I, NI, Gamma);
	Tg = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, p);
	
	/* Prob. de caer en cada intervalo segun Normal(μ,σ) */
	act4_gen_pi (p, I, NI, Normal);
	Tn = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, p);
	
	/* Prob. de caer en cada intervalo segun LogNormal(μ,σ) */
	act4_gen_pi (p, I, NI, LogNormal);
	Tln = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, p);
	
	printf ("Estadisticos segun Ji-2:\n\tGamma T = %.8f\n"
		"\tNorm T = %.8f\n\tLogNorm T = %.8f\n", Tg, Tn, Tln);
	
	printf ("\nValores-p segun Ji-2:\n");
	
	/* Gamma */
	p_value = chi_cuadrada (FRDG, Tg);
	printf ("\tGamma p-val = %.8f\n", p_value);
	
	/* Normal */
	p_value = chi_cuadrada (FRDN, Tn);
	printf ("\tNorm p-val = %.8f\n", p_value);
	
	/* LogNormal */
	p_value = chi_cuadrada (FRDLN, Tln);
	printf ("\tLogNorm p-val = %.8f\n", p_value);
	
	
	/** Calculamos los estadisticos de las distribuciones segun K-S */
	for (i=0)
	act4_monte_carlo ();
	double kolmogorov_smirnov (double *sample, unsigned int n, double (*F) (double));
	
	
	return 0;
}
