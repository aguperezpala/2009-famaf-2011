#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <err.h>
#include "rdg.h"
#include "permuter.h"


#define MAXC 1000  /* Máximo nº tolerable de cartas en un mazo */


/* Inicializa un arreglo de enteros con el formato [0,1,2,...,n-1] */
static void init_int_array (int *a, unsigned int n)
{
	unsigned int i=0;
	for (i=0 ; i<n ; i++) {
		a[i] = (int) i;
	}
	return;
}


int main (int argc, char **argv)
{
	unsigned int i=0, j=0;	/* Contadores misceláneos */
	int *deck=NULL;		/* Mazo de cartas */
	unsigned long   n=0,	/* nº de cartas de la baraja */
			N=0;	/* nº de repeticiones del experimento */
	char *err=NULL;
	double E=0.0, E2=0.0, V=0.0;	/* E[X], E[X^2] y V[X] */
	int X=0;			/* # de exitos de 1 experimento */
	double Eacc=0.0, E2acc=0.0;	/* Acumuladores de exitos*/
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar n=\"nº de cartas de la baraja"
				"\" y N=\"nº de repeticiones del experimento\"\n");
		return -1;
	} else {
		/* Obtenemos los parámetros ingresados */
		n = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar n=\"nº de cartas de la "
					"baraja\" como 1º parámetro\n");
			return -1;
		}
		
		N = (unsigned long) strtol (argv[2], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"nº de repeticiones del"
					"experimento\" como 2º parámetro\n");
			return -1;
			
		} else if (n > MAXC) {
			warn ("Truncando el nº de cartas al máximo tolerable "
					"(%d)\n", MAXC);
			n = MAXC;
		}
	}
	
	/* Generamos el mazo y lo mezclamos*/
	deck = (int *) malloc (n * sizeof(int));
	assert (deck != NULL);
	init_int_array (deck, n);

	X = 0;
	Eacc = 0.0;
	E2acc = 0.0;
	for (j=0 ; j<N ; j++) {
		/* Mezclamos el mazo */
		perm_int (deck, n);
		X = 0;
		/* Realizamos 1 experimento */
		for (i=0 ; i<n ; i++) {
			if (deck[i] == (int) i) X++;
		}
#ifdef _DEBUG
	printf ("Exitos: %d\n", X);
		printf ("Arreglo permutado: [");
		for (i=0 ; i<n ; i++){
			printf ("%d, ",deck[i]);
		}
		printf ("]\n");
#endif
		Eacc  += (double) X;
		E2acc += (double) X*X;
	}
	E  = Eacc/ (double) N;		/* E[X] = #(exitos) / #(experimentos) */
	E2 = E2acc/(double) N;		/* E[X^2] = #(exitos^2) / #(experimentos) */
	V  = E2-E*E;			/* V[X] = E[X^2] - (E[X])^2 */
	
	printf ("Con un mazo de %lu cartas y realizando %lu experimentos "
		"se estiman los valores:\nE[X] = %.6f\nV[X] = %.6f\n"
		, n, N, E, V);
	
	free (deck);
	deck = NULL;
		
	return 0;
}
