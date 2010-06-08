#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include "rdg.h"

#define PAD 40	/* Definido para arquitectura de 64 bits */


int miin (int argc, char **argv);



int miin (int argc, char **argv)
{
	unsigned int i=0;
	unsigned long N=0;
	long idum = 0;			/* Para inicialización de ran2 */
	struct timeval tv;		/* Para inicialización de ran2 */
	char *err=NULL;
	double r=0.0, o=0.0;		/* Para "arrojar la aguja" */
	double ya=0.0, yb=0.0;		/* Extremos izq y der de la aguja */
	double est=0.0;
	clock_t start = 0, end = 0;	/* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc!=2) {
		fprintf (stderr, "Debe especificar N=\"nº de iteraciones del"
		" método\"\n");
		return -1;
	} else {
		/* Obtenemos el nº de iteraciones 'N' */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"nº de iteraciones del"
			" método\" como unico parámetro\n");
			return -1;
		}
	}
		
	/* Para inicializacion de ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	/* Comenzamos a arrojar agujas */
	est = 0.0;
	start = clock();
	for (i=0 ; i<N ; i++) {
		r = ran2(&idum)*2.0;	/* Centro de la aguja */
		o = ran2(&idum)*M_PI;	/* Angulo de inclinacion */
		ya = r + 0.5*cos(o);
		yb = r - 0.5*cos(o);
		if (ya<0.0 || ya>2.0 || yb<0.0 || yb>2.0)
			est += 1.0;
	}
	est = (double) N / est; /* Pi es la inversa de la prop. calculada */
	end = clock();
	
	printf (">>> La estimación calculada es: %.8f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
