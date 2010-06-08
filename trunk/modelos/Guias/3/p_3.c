#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "rdg.h"

#define PAD 40	/* Definido para arquitectura de 64 bits */

int muin (int argc, char **argv);

int muin (int argc, char **argv)
{
	unsigned int i=0;
	unsigned long N=0, n=0;
	long idum = 0;			/* Para inicialización de ran2 */
	struct timeval tv;		/* Para inicialización de ran2 */
	char *err=NULL;
	double S=0.0, est=0.0;
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
	
	/* Estimamos el promedio */
	est = 0.0;
	start = clock();
	for (i=0 ; i<N ; i++) {
		S = 0.0;
		n = 0;
		while (S < 1.0) {
			S += ran2 (&idum);
			n++;
		}
		est += (double) n;
	}
	est = est / (double) N;
	end = clock();
	
	printf (">>> La estimación calculada es: %.8f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
