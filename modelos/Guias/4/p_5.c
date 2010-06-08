#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>	/* Para pow y M_E */
#include <limits.h>	/* Para ULONG_MAX */
#include <assert.h>
#include <stdbool.h>
#include <sys/time.h>	/* Para medición de tiempo */
#include <time.h>
#include "rdg.h"	/* Para mzran13 */


#define PAD	40	/* Definido para arquitectura de 64 bits */
#define LAMBDA	2.71828182845904523536028


bool	INIT=false;
double	mu_fact = 0.0,	/* mu! */
	lda_mu	= 0.0;	/* lambda^mu */


static double fact (unsigned long j)
{
	unsigned long i=j, res=1;
	for (i=j ; i>0 ; i--)
		res *= i;
	return (double) res;
}


static double Dsum (unsigned long k, double lda)
{
	unsigned int i=0;
	double res=0.0, fact[2]={1.0,1.0};
	res += pow (lda, 0.0) / fact[0];
	
	for (i=1 ; i<=k ; i++) {
		fact[i%2] = fact[(i+1)%2] * i;
		res += pow (lda, (double) i) / fact[i%2];
#ifdef _VERBOSE
		printf ("fact[%d]=%.1f\n", i, fact[i%2]);
		printf ("Dsum'%d=%.5f\n", i, res);
#endif
	}
	return res;
}


/* Devuelve el "limite" pi/(c*qi) de la condicion del ciclo del metodo de
 * aceptacion y rechazo, que en el papel vimos que es equivalente a
 * (LAMBDA^i/i!) * (mu!/LAMBDA^mu)
 */
static double calc_bound (double i, double mu)
{
	if (!INIT) {
		mu_fact = fact (mu);
		lda_mu = pow (LAMBDA, mu);
		INIT = true;
	}
	return (pow (LAMBDA, i) / fact(i)) * (mu_fact/lda_mu);
}



int main (int argc, char **argv)
{
	unsigned int I=0;
	unsigned long method=0, k=0;
	double U=0.0, p=0.0, F=0.0, Del=0.0;
	char *err=NULL;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;

	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar un valor para k "
				"y luego seleccionar el método a usar:\n"
				"[1] Método de la transformada inversa\n"
				"[2] Método de aceptación y rechazo\n");
		return -1;
	} else {
		/* Obtenemos el valor elegido para k */
		k = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar k=\"límite superior de la "
					" sumatoria\" como primer parámetro\n");
			return -1;
		}
		/* Obtenemos el método de generación elegido */
		method = (unsigned long) strtol (argv[2], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar el método de generación "
					" {1,2} como segundo parámetro\n");
			return -1;
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	/* Comenzamos a generar el valor */
	if (method == 1) {			/** NOTE Transformada inversa */
		start = clock();
		Del = Dsum (k, LAMBDA);
		assert (Del != 0.0);
		
		U = ran2(&idum);
		I = 0;
		p = 1.0/Del;
		F = p;
#ifdef _VERBOSE
		printf ("U=%.5f\tp=%.5f\tF=%.5f\n", U, p, F);
#endif
		while (U>=F) {
			p = p * (LAMBDA / (double)(I+1));	/* P(X=I+1) */
			F += p;					/* F(I+1) */
			I++;
#ifdef _VERBOSE
			printf ("Iteration %d\nU=%.5f\tp=%.5f\tF=%.5f\n", I, U, p, F);
#endif
		}
		/* El I final sera nuestra v.a. generada */
		end = clock();
		
	} else if (method == 2) {		/** NOTE Aceptacion y rechazo */
		double V=0.0, mu=floor(LAMBDA), bound=0.0;
		start = clock();
		do {
			U = ran2(&idum);
			I = floor (((double)k+1.0) * U);
			V = ran2(&idum);
			bound = calc_bound(I,mu);	/* bound = pi/(c*qi) */
		} while (V > bound);
		/* El I final sera nuestra v.a. generada */
		end = clock();
		
	} else {
		fprintf (stderr, "Debe pasar el método de generación "
		" {1,2} como segundo parámetro\n");
		return -1;
	}
	
	printf (">>> X = %d\n",I);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
