#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rdg.h"


static double f (double k, double N) { return pow (M_E, k/N);}


int main (int argc, char **argv)
{
	unsigned int j=0;
	unsigned long n=0, N=0, Ij=0;
	double est=0.0, real=0.0;
	char *err=NULL;
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar el 'N' de la suma total y el"
		" 'n' de la suma aproximada\n");
		return -1;
	} else {
		/* Obtenemos los parámetros ingresados */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar el 'N' de la suma total"
			" como 1º parámetro\n");
			return -1;
		}
		
		n = (unsigned long) strtol (argv[2], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar el 'n' de la suma "
			"aproximada como 2º parámetro\n");
			return -1;
			
		} else if (n >= N) {
			printf ("Sos vivo?, n>N? Se acabo, me voy de aqui\n");
			return -1;
		}
	}
	
	/* Hacemos solo n iteraciones para aproximar la suma hasta N */
	for (j=0 ; j<n ; j++) {
		Ij = (mzran13() % N) + 1;
		est += f ((double) Ij, (double) N);
	}
	/*est = (est*(double)N) / (double)n;*/
	est = est * n;
	
	/* Para poder comparar calculamos la suma verdadera */
	for (j=0 ; j<N ; j++)
		real += f ((double) j, (double) N);
	
	printf ("La estimacion de la sumatoria es: %.8f\n", est);
	printf ("Aproxima al valor real de la sumatoria con un error cercano"
		" a %.8f\n", /*fabs(real-est)*/ real);
	
	return 0;
}
