#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */



/* Genera una v.a. segun el metodo de la composicion
 * La regla de asignacion es:
 * P(X=i) = 1/2 * P(X1=i) + 1/2 * P(X2=i)
 * donde: X1 ~ Geom (1/2)
 *	  X2 ~ Geom (1/3)
 */
int main (int argc, char **argv)
{
	int N = 0, i = 0, X = 0;
	double U = 0.0, E = 0.0;
	char *err=NULL;
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;
	
	if (argc != 2) {
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
	
	E = 0.0;
	for (i=0 ; i<N ; i++) {
		U = ran2 (&idum);
		if (U < 0.5) {
			/* Geom (1/2) */
			U = ran2(&idum);
			X = floor (log (U) / log (0.5)) + 1.0;
		} else {
			/* Geom (1/3) */
			U = ran2(&idum);
			X = floor (log (U) / log (1.0-1.0/3.0)) + 1.0;
		}
		printf ("%d\n", X);
		E += (double) X;
	}
	
	E  = E / (double) N;	/* E[X] */
	printf ("E[X] = %.5f\n", E);
	
	return 0;
}
