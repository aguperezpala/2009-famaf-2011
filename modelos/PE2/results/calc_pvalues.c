#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "../rdg.h"
#include "../ssv.h"
#include "../file_reader.h"



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


/* Funcion de densidad de una Gamma(α,β) con α ~ 8.913 y β ~ 0.0571457 */
static double Gamma (double x)
{
	static double	a = 8.913,	/* α */
			b = 0.0571457,	/* β */
			c = 119951465170.848193661/33483.67682051149;
	
	assert (x > 0.0);
	
	return pow (x, a-1.0) * exp (-x/b) * c;
}


/* Funcion de densidad de una Normal(μ,σ) con μ ~ 0.50934599 y σ ~ 0.17069248 */
static double Normal (double x)
{
	return 2.3372 * exp(-17.1609 * pow (x-0.509346, 2.0));
}


/* Funcion densidad de una LogNormal(μ,σ) con μ ~ -0.7312493 y σ ~ 0.3418444 */
static double Normal (double x)
{
	return 1.16718 * exp (-4.27982 * pow (ln(x) + 0.7312, 2.0)) / x;
}



int main (void)
{
	double p_value = 0.0;
	
	double I[NI];	/* Intervalos */
	double p[NI];	/* Probabilidades teoricas de caer en los intervalos */
	
	double sample[NI];	/* Valores muestrales */
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
	
	/* Calculamos los estadisticos de las distribuciones segun Ji-2 */
	Tg = ji_cuadrado (sample, SAMPLE_SIZE, I, NI, p);
	
	
	
	return 0;
}
