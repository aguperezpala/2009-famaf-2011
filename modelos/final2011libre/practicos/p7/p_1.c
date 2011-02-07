#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"


/* generamos una muestra de n valores de una variable aleatoria que cumpla
 * son la siguiente func de prob: p1 = 1/4, p2 = 1/2, p3 = 1/4.
 * Osea puede generar 3 valores, 1 2 o 3
 */
void genera_muestra_mendel(double *sample, int size)
{
	double U = 0;
	int i = 0;
	
	for(i = 0; i < size; i++) {
		U = rg_gen_uni_cont(0,1);
		if(U < 0.5)
			sample[i] = 1.5;
		else if(U < 0.75)
			sample[i] = 0.5;
		else
			sample[i] = 2.5;
	}
}


/* Ver como resolvio carlox, ta mas prolijo */

int main1 (void)
{
	double pValor = 0.0;
	int i = 0;
	double samples[564];	/* este es el tamanio de la muestra observada
				 que para la simulacion tenemos que generar igual
				 cantidad de datos */
	double I[3] = {0,1,2}; /* esto es de 0 a 2) osea, x == 1, de 2 a 3 osea x == 2, etc.*/
	double p[3] = {0.25, 0.5, 0.25};
	int k = 3;
	int n = 564;
	double t = 0;
	double tObservado = 0.86170213; 	/* esto fue calculado a manopla :D */
	int counter = 0;
	
	for(i = 0; i < 100000; i++) {
		genera_muestra_mendel(samples, 564);
		/* simulamos */
		t = ji_cuadrado (samples, n, I, k, p);
		if(t > tObservado)
			counter++;
	}
	
	/* Calculamos la proporcion de t mayores que el observado*/
	pValor = ((double)counter)/100000.0;
	
	printf("p-valor: %G\n", pValor);
	
	return 0;
}
