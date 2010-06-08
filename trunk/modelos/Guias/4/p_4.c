#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>	/* Para pow y M_E */
#include <limits.h>	/* Para ULONG_MAX */
#include <sys/time.h>	/* Para medición de tiempo */
#include <time.h>
#include "rdg.h"	/* Para mzran13 */

/* Funcion de la E[|Z|] a integrar */
static double f (double y) {
	return ((1.0/y)-1.0) * pow (M_E, pow ((1.0/y)-1.0, 2.0) / (-2.0)) * \
			 1.0/(pow (y, 2.0)); }

	
int main (int argc, char **argv)
{
	unsigned int i=0;
	unsigned long N=0;
	double U=0.0, est=0.0;
	char *err=NULL;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc!=2) {
		fprintf (stderr, "Debe especificar algun nº de iteraciones \n");
		return -1;
	} else {
		/* Obtenemos el nº de iteraciones 'N' */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"nº de iteraciones del"
					" método\" como único parámetro\n");
			return -1;
		}
	}
	
	/* Estimamos la integral */
	est = 0.0;
	start = clock();
	for (i=0 ; i<N ; i++) {
		U = mzran13() / (double) ULONG_MAX;
		est += f(U);
	}
	est = sqrt(M_2_PI) * est / (double) N;		/* M_2_PI == 2/PI */
	end = clock();
	
	printf (">>> La estimación calculada para E[|Z|] es: %.12f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
