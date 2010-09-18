#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include "mzran13.h"


/* # de neuronas de la red */
#define  N  10
/* # de memorias asociativas (aka "atractores") */
#define  p  5
/* Máximo # de veces que permitimos actualizar el estado S */
#define  MAX_ITER  20

/* Normalización a (-1,+1) */
#define  norm(x)  ((x) > 0 ? 1 : -1)

#define  printsep();  printf ("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"\
			      "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");


/* Neuronas de Hopfield */
short S[N];
/* Memorias asociativas (aka "atractores") */
int XI[p][N];
/* Matriz sinaptica */
long int W[N][N];
/* NOTE La matriz sinaptica se mantendra como suma de enteros sin el factor 1/N
 * A la hora de utilizar un valor W[i][j] primero se lo debe multiplicar por nn
 */
double nn = 1.0 / (double)N;



/* Inicializa aleatoriamente las p memorias en XI */
static void init_mems (void)
{
	long i = 0;
	uint64_t aux = 0;
	
	#pragma omp parallel for shared(XI)
	for (i=0 ; i<N*p ; i++) {
		aux = mzran13() % 2;
		XI[i/N][i%N] = norm(aux);
	}
	
	return;
}

static void print_mems (void)
{
	unsigned int mu=0, j=0;
	
	for (mu=0 ; mu<p ; mu++) {
		printf ("\nMemoria #%d:\n[ ", mu);
		for (j=0 ; j<N ; j++)
			printf ("%+d ", XI[mu][j]);
		printf ("]\n");
	}
	
	return;
}


static void init_sinaptic_mesh (void)
{
	long i=0, j=0, mu=0, tmp=0;
	
	/* La siguiente linea es necesaria para que la compilacion
	 * con la opcion -O3 no haga cagadas */
	i = i;
	#pragma omp parallel for shared(W,XI) private(i,j,mu,tmp)
	for (i=0 ; i<N ; i++) {
		for (j=0 ; j<i ; j++) {
			W[i][j] = W[j][i] = 0.0;
			for (mu=0 ; mu<p ; mu++) {
				tmp = XI[mu][i] * XI[mu][j];
				W[i][j] += tmp;
				W[j][i] += tmp;
			}
		}
		/* assert (j == i); */
		W[i][j] = 0;
	}
	
	return;
}

static void print_sinaptic_mesh (void)
{
	unsigned int i=0, j=0;
	double n = N;
	
	printf ("\nSinaptic mesh W[i][j]\n");
	for (i=0 ; i<N ; i++) {
		for (j=0 ; j<N ; j++)
			printf ("%.2f\t", (double) W[i][j] / n);
		printf ("\n");
	}
	printf ("\n");
	
	return;
}



/* Actualiza el estado de la i-ésima neurona en 1 intervalo temporal
 *
 * NOTE: Versión DETERMINÍSTICA
 *
 * PRE: i < N
 * POS: S[i] fue actualizada según la matriz sináptica W[i][*]
 */
static void neurona_det (unsigned int i)
{
	long j = 0;
	double h = 0.0;
	
	assert (i < N);
	
	#pragma omp parallel for shared(W,S,i) reduction(+:h)
	for (j=0 ; j<N ; j++)
		h += nn * ((double) W[i][j]) * ((double) S[j]);
	S[i] = h >= 0.0 ? 1 : -1;
	
	return;
}


/* Actualiza el estado de la i-ésima neurona en 1 intervalo temporal
 *
 * NOTE: Versión ESTOCÁSTICA
 *
 * PRE: i < N
 * POS: S[i] fue actualizada según la matriz sináptica W[i][*]
 */
static void neurona_est (unsigned int i)
{
	long j = 0;
	double	h = 0.0,
		prob = 0.0,
		z = 0.0;
	static double	beta = (double)N / (2.0*M_SQRT2*(double)p);
	
	assert (i < N);
	
	#pragma omp parallel for shared(W,S,i) reduction(+:h)
	for (j=0 ; j<N ; j++)
		h += nn * ((double) W[i][j]) * ((double) S[j]);
	
	prob = (1.0+tanh(beta*h))/2.0;
	z = (double)mzran13()/(double)ULONG_MAX;
	
	S[i] = z < prob ? 1 : -1;
	
	return;
}


int main (void)
{
	unsigned int i=0, k=0;
	
	/* Inicializamos las memorias */
	init_mems  ();
	print_mems ();
	printsep ();
	
	/* Inicializamos el estado */
	init_sinaptic_mesh  ();
	print_sinaptic_mesh ();
	printsep ();
	
	/* Hacemos algunas actualizaciones determinísticas */
	for (k=0 ; k<MAX_ITER/2 ; k++) {
		printf ("\nEstado %u\t\t\t\t(DET)\n[ ", k);
		for (i=0 ; i<N ; i++) {
			neurona_det(i);
			printf ("%+d ", S[i]);
		}
		printf ("]\n");
	}
	
	printsep();
	
	/* Hacemos otras actualizaciones estocásticas */
	for (; k<MAX_ITER ; k++) {
		printf ("\nEstado %u\t\t\t\t(EST)\n[ ", k);
		for (i=0 ; i<N ; i++) {
			neurona_est(i);
			printf ("%+d ", S[i]);
		}
		printf ("]\n");
	}
	
	return 0;
}
