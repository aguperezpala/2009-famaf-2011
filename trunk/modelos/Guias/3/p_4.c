#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include "rdg.h"

#define PAD 40	/* Definido para arquitectura de 64 bits */


int moin (int argc, char **argv);



int moin (int argc, char **argv)
{
	unsigned int i=0, n=0;
	unsigned long N=0, k=0;
	long idum = 0;			/* Para inicialización de ran2 */
	struct timeval tv;		/* Para inicialización de ran2 */
	char *err=NULL;
	double S=0.0, est=0.0, upBound=0.0;
	clock_t start = 0, end = 0;	/* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc < 3) {
		fprintf (stderr, "Debe especificar el inciso {a,b} a resolver"
				" y N=\"nº de iteraciones del método\"\n");
		return -1;
	} else {
		/* Obtenemos el nº de iteraciones 'N' */
		N = (unsigned long) strtol (argv[2], &err, 10);
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
	
	est = 0.0;
	upBound = pow (M_E, -3.0);	/* upBound = e^(-3) ~ 0,0497870684 */
	
	/* Vemos cual inciso escogieron */
	assert (argv[1] != NULL);
	if ( strnlen(argv[1],2) > 1) {
		printf ("Error en \'%s\'\n",argv[1]);
		fprintf (stderr, "Debe pasar el inciso a resolver {a,b} "
				"como primer parámetro\n");
		return -1;
		
	} else if (argv[1][0] == 'a') {
		/* Calculamos la esperanza de N */
		start = clock();
		for (i=0 ; i<N ; i++) {
			S = 1.0;
			k = 0;
			while (S >= upBound) {
				S *= ran2 (&idum);
				k++;
			}
			/* Aqui ya S < upBound => nos pasamos por uno =>
			 * => queremos el anterior, que es k-1 */
			est += (double) k-1;
		}
		est = est / (double) N;
		end = clock();
		
	} else if (argv[1][0] == 'b') {
		/* Calculamos la probabilidad de que N == n == argv[3] */
		if (argc!=4) {
			fprintf (stderr, "Debe pasar como tercer parametro el "
				"valor 'n' deseado de N\n");
				return -1;
		} else {
			/* Obtenemos 'n', el valor deseado para N */
			n = (unsigned int) strtol (argv[3], &err, 10);
			if (err[0] != '\0') {
				printf ("Error en \'%s\'\n",err);
				fprintf (stderr, "Debe pasar n=\"valor deseado "
					" de N\" como tercer parámetro\n");
				return -1;
			}
		}
		start = clock();
		for (i=0 ; i<N ; i++) {
			S = 1.0;
			k = 0;
			while (S >= upBound) {
				S *= ran2 (&idum);
				k++;
			}
			/* Aqui ya S < upBound => nos pasamos por uno =>
			* => queremos el anterior, que es k-1 */
			if (k-1 == n)
				est += 1.0; /* Exito */
		}
		est = est / (double) N;
		end = clock();
	
	} else {
		printf ("Error en '%s'\n", argv[1]);
		fprintf (stderr, "Debe pasar el inciso a resolver {a,b} "
			"como primer parametro\n");
		return -1;
	}
	
	printf (">>> La estimación calculada es: %.8f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
