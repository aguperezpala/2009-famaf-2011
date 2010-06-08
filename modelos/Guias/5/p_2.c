#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */


/* Genera una v.a. X ~ Weibull(alpha,beta) = W() */
int main (int argc, char **argv)
{
	int N = 0, i = 0;
	double X = 0.0, U = 0.0, E = 0.0;
	char *err=NULL;
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;
	
	if (argc != 4) {
		fprintf (stderr, "Debe especificar un # de valores a generar\n");
		return -1;
	} else {
		/* Obtenemos el # de valores que quiere */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar k=\"# de valores a generar"
			"\" como unico parámetro\n");
			return -1;
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	for (i=0 ; i<N ; i++) {
		U = ran2(&idum);
		if (U < 0.25)
			X = 2.0 + 2.0 * sqrt(U);
		else
			X = 6.0 * (1.0 - sqrt((1.0-U)/3.0));
/*		printf ("%.5f\n", X);
*/		E += X;
	}
	
	E = E / (double) N;	/* E[X] */
	printf ("E[X] = %.5f\n", E);
	
	return 0;
}
