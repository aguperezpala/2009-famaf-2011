#include <stdio.h>
#include <math.h>
#include "ssv.h"

/* Tamaño de la muestra */
#define  N  10
/* Extremos del intervalo */
double	a = -5.0,
	b =  5.0;

int main (void)
{
	/* Valores muestrales */
	double sample[N] = { 56.0, 101., 78.0, 67.0, 93.0,
			     87.0, 64.0, 72.0, 80.0, 69.0};
	/* Varianza del estimador */
	double ECM = 0.0;
	
	ECM = bootstrap_media (sample, N);
	
	printf ("A = %.8f\tB = %.8f\n",
		a / sqrt (ECM / (double)N), b / sqrt (ECM / (double)N));
	
	return 0;
}
