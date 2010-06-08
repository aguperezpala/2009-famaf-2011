#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */

double	Ta = 5.0,
	Tc = 3.0;


int main (int argc, char **argv)
{
	double t = 0.0;		/* Tiempo transcurrido durante una simulac */
	double tacc = 0.0;
	double tA = 0.0, tC = 0.0;
	int N = 0;
	bool looping = true;
	char *err = NULL;
	struct timeval tv;
	long idum = 0;
	int i = 0;
	double U = 0.0;
	
	/* Leemos la entrada y obtenemos los valores de u, s, y, N */
	if (argc != 2) {
		fprintf (stderr, "Debe indicar un # de simulaciones\n");
		return -1;
	} else {
		/* Obtenemos el valor elegido para la media u */
		N = strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar u=\"Media de la v.a.\""
			"como primer parametro\n");
			exit (EXIT_FAILURE);
		}
	}
		
		
	/* Inicializamos ran2 (1 sola vez) */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
		
	tacc = 0.0;
	for (i=0 ; i<N ; i++) {
		looping = true;
		t = 0.0;
		while (looping) {
			/* Ruteamos en A */
			U = ran2 (&idum);
			tA = -1.0/Ta * log(U);
			t += tA;
			
			/* Vemos si vamos hacia B o hacia C */
			U = ran2(&idum);
			if (U < 1.0/3.0)
				/* Nos fuimos hacia B y terminamos */
				looping = false;
			else {
				/* Nos fuimos hacia C */
				U = ran2 (&idum);
				tC = -1.0/Tc * log(U);
				t += tC;
				
				/* Vemos si vamos hacia B o hacia A */
				U = ran2(&idum);
				if (U < 0.5)
					/* nos fuimos hacia B y terminamos */
					looping = false;
				
				 /* else : volvimos hacia A */
			}
		}
/*		printf ("t = %.4f\n", t);
*/		
		tacc += t;	/* Acumulamos el nuevo resultado */
	}
	
	printf ("Tiempo medio de ruteo de un paquete: %.6f ms\n", tacc/(double) N);
	
	return 0;
}
