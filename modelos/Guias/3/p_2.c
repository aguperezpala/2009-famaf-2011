#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "rdg.h"


#define PAD 40	/* Definido para arquitectura de 64 bits */


/************************* Funciones para integrar ****************************/


static double p_2_a (float x) { return pow (1.0-x*x, 1.5 ); }


static double p_2_b (float x) {
	return ((1.0/x)-1.0) * pow (1.0 + pow ((1.0/x)-1, 2.0), -2.0) * (1.0/(x*x)); }


static double p_2_c (float x) {
	return 2.0 * pow (M_E, -(pow ((1.0/x)-1.0, 2.0))) * (1.0/(x*x)); }


static double p_2_d (float x, float y) {
	return pow (M_E, pow (x+y, 2.0)); }


static double p_2_e (float u, float v) {
	return pow (M_E, 2.0-(1.0/u)-(1.0/v)) * 1.0/(v*v) * 1.0/(u*u); }



/**************************** Rutina estimadora *******************************/

int mein (int argc, char *argv[]);


int mein (int argc, char *argv[])
{
	unsigned int  i=0;
	unsigned long N=0;	/* Total de iteraciones a realizar */
	long idum = 0;		/* Para inicialización de ran2 */
	struct timeval tv;	/* Para inicialización de ran2 */
	float U=0.0, V=0.0;	/* v.a. 'U','V' de distr. uniforme(0,1) */
	double est=0.0;		/* Estimación 'est' de la integral */
	double (*f)(float);	/* Puntero a la función simple 'f' a integrar */
	double (*g)(float,float); /* Puntero a la función doble 'g' a integrar */
	char *err=NULL;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar N=\"nº de iteraciones del "
				"método\", y el inciso [a..e] a resolver\n");
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
		fprintf (stderr, "Debe pasar el inciso a resolver [a..e] (una "
				"sola letra) como segundo parámetro\n");
		return -1;
	} else {
		switch ((char) argv[2][0]) {
		
		case 'a':
			f = p_2_a;
			break;
		case 'b':
			f = p_2_b;
			break;
		case 'c':
			f = p_2_c;
			break;
		case 'd':
			g = p_2_d;
			break;
		case 'e':
			g = p_2_e;
			break;
		default:
			printf ("Error en \'%s\'\n",argv[2]);
			fprintf (stderr, "Debe pasar el inciso a resolver [a..e] "
					"como segundo parámetro\n");
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
	if (f==p_2_a || f==p_2_b || f==p_2_c) {	/** Integrales simples */
		start = clock();
		for (i=0 ; i<N ; i++) {
/*			U = mzran13() / (double) ULONG_MAX;
			printf ("U[%d] = %.5f\n", i, U);
*/			U = ran2(&idum);
			est += f(U);
		}
		est = est / ((double) N);
		end = clock();
	} else if (g==p_2_d) {			/** Integrales dobles */
		start = clock();
		for (i=0 ; i<N ; i++) {
			U = ran2(&idum);
			V = ran2(&idum);
			est += g(U,V);
		}
		est = est / ((double) N);
		end = clock();
	} else if (g==p_2_e) {
		start = clock();
		for (i=0 ; i<N ; i++) {
			U = ran2(&idum);
			V = ran2(&idum);
			if (V>U)
				est += g(U,V);
		}
		est = est / ((double) N);
		end = clock();
	} else {
		fprintf (stderr, "Error interno del programa. Abortando\n");
		return -1;
	}
	  
	printf (">>> La estimación calculada es: %.12f\n",est);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
