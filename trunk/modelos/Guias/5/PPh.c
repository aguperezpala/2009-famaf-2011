
/* Proceso de Poisson homogeneo */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */
#define MAXEV	200	/* Maximo numero de eventos permitidos */



int main (int argc, char **argv)
{
	double	T = 0.0,	/* Intervalo total del proceso = [0..T]  */
		lambda = 0.0;	/* Razon del proceso: N(T) ~ P(lambda*T) */
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;
	char *err = NULL;
	int I = 0;		/* # de evento actual */
	double  U = 0.0,
		ev = 0.0,	/* Nuevo evento en el tiempo ~ Exp(lambda) */
		t = 0.0,	/* Tiempo actual */
		S[MAXEV];	/* Tiempo registrado para los eventos */
	bool finished = false;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	double elapsed = 0.0;
	int i = 0;
	
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar los valores de T y lambda, "
				"en ese orden,\npara que el proceso generado "
				"sea N(T) con razon lambda\n");
		exit (EXIT_FAILURE);
	} else {
		/* Obtenemos el valor elegido para T */
		T = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar T=\"límite superior de "
			"tiempo del proceso\" como primer parametro\n");
			exit (EXIT_FAILURE);
		}
		/* Obtenemos el valor elegido para lambda */
		lambda = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar lambda=\"razon del proceso"
					"\" como segundo parametro\n");
			exit (EXIT_FAILURE);
		}
	}
	
	/* Inicializamos */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	t = 0.0;
	I = 0;
	memset (S, 0.0, MAXEV);
	
	start = clock();
	
	/* Algoritmo */
	while (!finished) {
		U = ran2 (&idum);
		ev = -1.0/lambda * log(U);
		
		if (t + ev > T)
			finished = true;
		else {
			t += ev;
			S[I] = t;
			I++;
		}
		if (I == MAXEV) {
			fprintf (stderr, "No se alcanzo el tiempo T = %.6f "
					"deseado, el numero de eventos\nhasta t "
					"= %.6f alcanzo el maximo tolerado (%d)"
					"\n", T, t, I);
			finished = true;
		}
	}
	
	end = clock();
	elapsed = (double) ((end-start+0.0)/CLOCKS_PER_SEC); 
	
	/* Impresion resultados */
	printf ("Finalizado el experimento. El tiempo total de calculo fue"
		" %.3f.\nLos resultados fueron:\n# de eventos = %d\n"
		"Tiempo de ocurrencia de los eventos:\n", elapsed, I);
	for (i=0 ; i<I ; i++) printf ("%d)\t%.8f\n", i+1, S[i]);
	
	return 0;
}
