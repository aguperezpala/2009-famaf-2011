#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rdg.h"


/* # de dados a usar */
#define DICES 2
/* # de lados de un dado */
#define EDGES 6
/* minimo # posible a obtener */
#define FLOOR DICES
/* maximo # posible a obtener */
#define ROOF DICES*EDGES



int main (int argc, char **argv)
{
	int D1=0, D2=0;		/* Dados 1 y 2 */
	int i=0, j=0;
	long idum=-1;		/* Inicializacion de ran2 */
	unsigned long N=0;	/* # de repeticiones del experimento */
	char *err=NULL;
	int hits[EDGES*DICES+1];	/* # que ya salieron durante un experimento */
	int X=0;		/* # de "aciertos" durante un experimento */
	int Eacc=0, E2acc=0;	/* Acumuladores de resultados */
	double E=0.0, E2=0.0, V=0.0;	/* E[X], E[X^2], V[X] */
	
	/* Manejo de la entrada */
	if (argc!=2) {
		fprintf (stderr, "Debe especificar el numero de veces que "
				"desea que se repita el experimento\n");
		return -1;
	} else {
		/* Obtenemos los parámetros ingresados */
		N = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar como 1º parametro el "
			"numero de veces que desea repetir el experimento\n");
			return -1;
		}
	}
	
	for (i=0 ; i<(int)N ; i++) {
		/* Reinicializamos el arreglo de aciertos */
		memset (hits, 0, sizeof(int)*(EDGES*DICES+1));
		X = 0;
		j = 0;
		while (X < ROOF-FLOOR+1) {
			D1 = floor (ran2(&idum) * EDGES) + 1;
			D2 = floor (ran2(&idum) * EDGES) + 1;
			if (hits[D1+D2]==0) {
				hits[D1+D2] = 1;
				X++;
			}
#ifdef _DEBUG
			printf ("D1 = %d\tD2 = %d\t\tX = %d\n", D1, D2, X);
#endif
			j++;
		}
		Eacc  += (double) j;
		E2acc += (double) j*j;
	}
	E  = Eacc / (double) N;	/* E[X] = #(exitos) / #(experimentos) */
	E2 = E2acc /(double) N;	/* E[X^2] = #(exitos^2) / #(experimentos) */
	V  = E2-E*E;		/* V[X] = E[X^2] - (E[X])^2 */
	
	printf ("Realizando %lu experimentos se obtuvieron los valores:\n"
	"E[X] = %.8f\nDesvEst[X] = %.8f\n", N, E, sqrt(V));
	
	return 0;
}
