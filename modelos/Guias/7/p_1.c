#include <stdio.h>
#include "rdg.h"
#include "ssv.h"


/* # de intervalos en los que se divide el rango de la muestra */
#define  NI  3

/* # de simulaciones a ejecutar para ajustar el p-valor */
#define  NSIM  10000


int main (void)
{
	unsigned int i = 0;
	/* Informacion de la prueba */
	double p[NI] = {0.25, 0.5, 0.25};	/* Probabilidades teoricas */
	unsigned int n = 564;			/* # de valores muestrales */
	unsigned int N[NI] = {141, 291, 132};	/* frecuencias muestrales  */
	
	
	
	return 0;
}
