#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "rdg.h"	/* Random Deviates Generators */


#define PAD 40	/* Definido para arquitectura de 64 bits */


int ma1in (int argc, char *argv[]);

/** Funciones a integrar */

static double a (double x) {
	return log(x)/(1.0-x) ; }


static double b (double y) {
	return 2.0 * pow ((1.0/y)-1.0, 2.0) * pow (M_E, -pow ((1.0/y)-1.0, 2.0)) * (1.0/(y*y)); }



/** Rutina estimadora de las integrales */

int ma1in (int argc, char *argv[])
{
	unsigned int  i=0;
	unsigned long N=0;	/* Total de iteraciones a realizar */
	long idum = 0;		/* Para inicialización de ran2 */
	struct timeval tv;	/* Para inicialización de ran2 */
	double U=0.0;		/* v.a. 'U' de distribucion uniforme (0,1) */
	double est=0.0;		/* Estimación 'est' de la integral */
	double (*f)(double);	/* Puntero a la función simple 'f' a integrar */
	char *err=NULL;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar N=\"nº de iteraciones del "
				"método\", y el inciso {a,b} a resolver\n");
		return -1;
	} else {
		/* Obtenemos el nº de iteraciones 'N' */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"nº de iteraciones del"
					" método\" como primer parámetro\n");
			return -1;
		}
	}
	/* Elegimos la función a integrar */
	assert (argv[2] != NULL);
	if ( strnlen(argv[2],2) > 1) {
		printf ("Error en \'%s\'\n",argv[2]);
		fprintf (stderr, "Debe pasar el inciso a resolver {a,b} (una "
				"sola letra) como segundo parámetro\n");
		return -1;
	} else {
		switch ((char) argv[2][0]) {
		
		case 'a':
			f = a;
			break;
		case 'b':
			f = b;
			break;
		default:
			printf ("Error en \'%s\'\n",argv[2]);
			fprintf (stderr, "Debe pasar el inciso a resolver {a,b}"
					" como segundo parámetro\n");
			return -1;
			break;
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	/* Estimamos la integral */
	est = 0.0;
	start = clock();
	for (i=0 ; i<N ; i++) {
/*		U = mzran13() / (double) ULONG_MAX;
		printf ("U[%d] = %.5f\n", i, U);
*/		U = ran2(&idum);
		est += f(U);
	}
	est = est / ((double) N);
	end = clock();
	
	
	printf (">>> La estimación calculada es: %.12f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
