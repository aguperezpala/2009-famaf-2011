#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */


/* Genera dos v.a. N(0,1) con el metodo polar */
int main (int argc, char **argv)
{
	int N = 0, i = 0;
	double	V1 = 0.0, V2 = 0.0,	/* Coordenadas del punto en el circulo */
		X = 0.0, Y = 0.0,	/* V.A. ~ N(0,1) a generar */
		S = 0.0,		/* V1^2 + V2^2 */
		E = 0.0, E2 = 0.0, V = 0.0;
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
			fprintf (stderr, "Debe pasar N=\"# de valores a generar"
					"\" como unico parámetro\n");
			sleep (2);
			fprintf (stderr, "Por favor, un solo parámetro pedimos,"
					" tan dificil es?\n");
			sleep (2);
			fprintf (stderr, "A ver, dale, proba de nuevo\n");
			sleep (1);
			return -1;
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	E  = 0.0;
	E2 = 0.0;
	for (i=0 ; i<N ; i++) {
		/* Generamos el punto dentro del circulo de radio 1 */
		do {
			V1 = ((ran2 (&idum)) * 2.0) - 1.0;
			V2 = ((ran2 (&idum)) * 2.0) - 1.0;
			/* V1,V2 ~ U(-1,1) */
			S = V1*V1 + V2*V2;
		} while (S > 1.0);
		
		X = sqrt (-2.0 * log(S) / S) * V1;
		Y = sqrt (-2.0 * log(S) / S) * V2;
		
		printf ("X = %.5f\tY = %.5f\n", V1, V2);
		
		E  +=  X  +  Y  ;
		E2 += X*X + Y*Y ;
	}
	
	E  = E  / (double) N;
	E2 = E2 / (double) N;
	V  = E2 - E*E;
	printf ("E[X] = %.5f\n", E);
	printf ("V[X] = %.5f\n", V);
	
	
	return 0;
}
