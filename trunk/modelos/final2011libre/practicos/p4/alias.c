/*
 * Metodo del alias para generar una variable aleatoria X con funcion de
 * distribucion de probabilidad p[0..N-1], donde X puede tomar cualquier valor
 * entre x[0..N-1]
 *
 * Estos datos son tomados del archivo cuyo nombre se pasa como parametro por
 * consola al programa.
 *
 * Formato de archivo:
 *	<Numero de valores posibles para X>
 *	<x0>    <p0>
 *	<x1>    <p1>
 *	...  ...
 *	<xN-1>  <pN-1>
 *
 * Ejemplo:
 * Para una variable X que puede tomar valores {12, 1, 99} con probabilidades
 * 0.33, 0.2553 y 0.4147 respectivamente el contenido del archivo debe ser:
 *
 * 3
 * 12	0.33
 * 1	0.2553
 * 99	0.4174
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"



#define PAD	40	/* Definido para arquitectura de 64 bits */
#define ITER	10	/* # de valores de la v.a. a generar */



/** NOTE Para implementacion con lectura de archivos */

#define DEBUG
#define FNLEN	50	/* Maxima longitud de nombre de archivo */
#define MAXVAL	10	/* El maximo # de valores que puede tomar la v.a.
			 * sera 10 ^ MAXVAL */



/** NOTE Para implementacion hardcodeada */

/* # total de valores posibles para nuestra v.a. X */
static unsigned int n = 4;

/* Vector de valores posibles para nuestra v.a. X */
static int x[] = { 1 , 2 , 3 , 4};

/* Probabilidades de X: p[i] = P(X = x[i]) */
static double p[] =  { 0.4375,
		0.25,
		0.125,
		0.1875
	      };


/* TODO
 * Parsea el archivo de nombre fname en busca de los valores x[k] que puede
 * tomar nuestra v.a., y las probabilidades p[k] asociadas
 * Genera memoria para 'x' y 'p' que debe ser liberada por el usuario
 */
int parse (char fname[FNLEN], int **xx, double **pp);
int parse (char fname[FNLEN], int **xx, double **pp)
{
	FILE *f = NULL;
	char aux[MAXVAL], *err = NULL;
	int N = 0, i = 0;
	
	/* Abrimos el archivo */
	f = fopen (fname, "r");
	if (f == NULL)
		return -1;
#ifdef DEBUG
	printf ("Leyendo archivo %s\n", fname);
#endif
	
	/* Leemos N = # de valores que puede tomar la v.a. < MAXVAL */
	fscanf (f, "%[^\n]s", (char *) &aux);
	N = strtol (aux, &err, 10);
	if (err[0] != '\0') {
		printf ("Error en \'%s\'\n",err);
		fprintf (stderr, "El 1º elemento del archivo de ser N = # de "
			"valores diferentes que puede tomar la v.a.\n");
		return -1;
	}

#ifdef DEBUG
	printf ("Se leeran %d valores para la v.a.\n", N);
#endif
	
	/* Generamos la memoria necesaria en los arreglos */
	*xx = (int *)   calloc (N, sizeof (int));
	*pp = (double*) calloc (N, sizeof (double));
	assert (xx != NULL && pp != NULL);
	
	/* Llenamos los arreglos con lo que leemos del archivo */
	for (i=0 ; i<N ; i++) {
		
		fgetc (f);	/* Comemos el ultimo '\n' */
		fscanf (f, "%[^\t ]s", (char *) &aux);
		/* TODO */
	}
	
	return 0;
}




int main (int argc, char **argv)
{
/**	Global variables in scope:					*
 *	int n;								*
 *	int x[n]		Valores posibles de la v.a. X		*
 *	double p[n]		Probabilidades p[i] = P (X = x[i])	*/
	unsigned int i = 0, j = 0, k = 0;
	double U = 0.0, V = 0.0, E = 0.0;
	int X = 0;
	int *alias = NULL;	/* Vector de aliases */
	double  *F = NULL;	/* Vector de probabilidades Bernoulli */
	int	*G = NULL,	/* Probabilidades > 1 */
		*S = NULL;	/* Probabilidades < 1 */
	int g = 0, s = 0;	/* Punteros para G y S */
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;
	
	
	/* TODO Version con lectura de archivo
	int fnlen = FNLEN;	 Max long. tolerada de nombre de archivo 
	char fname[FNLEN];	 Nombre del archivo con los x[k] y los p[k] 
	int *x = NULL;		 Vector de valores de la v.a. (x[k]) 
	double *p = NULL;	 Vector de probabilidades de la v.a. (p[k]) 
	int err = 0;
	
	if (argc != 2) {
		fprintf (stderr, "Debe pasar como unico argumento el nombre del"
			" archivo que contiene los datos de la distribucion\n"
			"Dicho nombre no debe superar los %d caracteres de "
			"longitud\n", fnlen);
		exit (EXIT_FAILURE);
	} else {
		memset (fname, '\0', FNLEN);
		scanf ("%s", fname);
		assert (fname != NULL);
	}
	err = parse (fname, &x, &p);
	if (err) {
		fprintf (stderr, "Error de lectura del archivo de datos\n");
		exit (EXIT_FAILURE);
	}*/
	
	
	/* Generamos los alias */
	
	alias = (int *) calloc (n, sizeof (int));
	F  = (double *) calloc (n, sizeof (double));
	G = (int *) calloc (n, sizeof (int));
	S = (int *) calloc (n, sizeof (int));
	assert (alias !=  NULL	&&
		F     !=  NULL	&&
		G     !=  NULL	&&
		S     != NULL);
	
	for (i=0 ; i<n ; i++) {	
		/* Calculamos las prob. Bernoulli */
		F[i] = n*p[i];
		
		/* Rellenamos los conjuntos G y S */
		if (F[i] > 1.0) {
			/* Este es grande */
			G[g] = i;
			g++;
			
		} else if (F[i] < 1.0) {
			/* Este es chico */
			S[s] = i;
			s++;
		}
	}
	while (s != 0) {
		/* Tomamos uno 'j' chico y un 'k' grande */
		j = S[s];
		s--;
		k = G[g];
		g--;
		
		/* x[k] es alias de x[j] */
		alias[j] = x[k];
		
		/* Actualizamos la probabilidad Bernoulli de x[k] */
		F[k] -= 1.0-F[j];
		
		if (F[k] < 1.0) {
			/* Ahora x[k] se volvio chico */
			S[s] = k;
			s++;
		
		} else if (F[k] > 1.0) {
			/* x[k] sigue siendo grande */
			G[g] = k;
			g++;
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	for (j=0 ; j<ITER ; j++) {
		
		U = ran2(&idum);
		V = ran2(&idum);
		
		i = floor(n*U);
		assert (i < n);
		if (V < F[i])
			X = x[i];
		else
			X = alias[i];
		printf ("%d\n", X);
		E += (double) X;
	}
	
	E  = E / (double) ITER;	/* E[X] */
	printf ("E[X] = %.5f\n", E);
	
	/* Version con lectura de archivo
	free (x);	x = NULL;
	free (p);	p = NULL;
	*/
	
	return 0;
}
