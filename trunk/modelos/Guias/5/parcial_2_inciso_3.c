#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */

/* Solo se inicializa una vez en el main */
static long idum = 1;


/* Par de normales estandard */
typedef struct {
	double v1;
	double v2;
} Znorm ;


static void genZ (Znorm *Z)
{
	double	V1 = 0.0, V2 = 0.0, S = 0.0;
		
	/* Generamos el punto dentro del circulo de radio 1 */
	do {
		V1 = ran2 (&idum);
		V2 = ran2 (&idum);
		S = V1*V1 + V2*V2;
	} while (S > 1.0);
	
/*	printf ("V1 = %.3f\nV2 = %.3f\n", V1, V2);	
*/	
	Z->v1 = sqrt (-2.0 * log(S) / S) * V1;
	Z->v2 = sqrt (-2.0 * log(S) / S) * V2;
	
	return;
}


/* Genera dos v.a. N(0,1) con el metodo polar */
int main (int argc, char **argv)
{
	int	N = 0,			/* # de simulaciones */
		i = 0,
		count = 0;		/* # de "exitos" */
	Znorm Z;			/* Dupla de v.a. ~ N(0,1) */
	double	u = 0.0,		/* Media de la v.a. ~ N(u,s) */
		s = 0.0,		/* Desv.Est de la v.a. ~ N(u,s) */
		y = 0.0,		/* Valor a acumular, calcularemos F(y) */
		z = 0.0;		/* (y - u) / s */
	char *err=NULL;
	struct timeval tv;
	
	
	/* Leemos la entrada y obtenemos los valores de u, s, y, N */
	if (argc != 5) {
		fprintf (stderr, "Debe pasar 4 parametros:\n"
				"1) La media de la v.a.\n"
				"2) La desviacion estandard de la v.a.\n"
				"3) El valor 'y' a \"acumular\"\n"
				"4) El # deseado de simulaciones\n");
		return -1;
	} else {
		/* Obtenemos el valor elegido para la media u */
		u = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar u=\"Media de la v.a.\""
					"como primer parametro\n");
			exit (EXIT_FAILURE);
		}
		/* Obtenemos el valor elegido para la desviacion estandard s */
		s = strtod (argv[2], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar s=\"Desviacion estandard "
			"de la v.a.\" como segundo parametro\n");
			exit (EXIT_FAILURE);
		}
		/* Obtenemos el valor elegido para y */
		y = strtod (argv[3], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar y=\"valor a acumular\" "
			"como segundo parametro, para que calculemos P(Y<y)\n");
			exit (EXIT_FAILURE);
		}
		/* Obtenemos el # de iteraciones que quiere */
		N = (unsigned long) strtol (argv[4], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"# de simulaciones a "
					"correr\" como ultimo parámetro\n");
			exit (EXIT_FAILURE);
		}
	}
	
	z = (y - u) / s;
	
	/* Inicializamos ran2 (1 sola vez) */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	/* Simulamos. Como cada llamado a genZ genera 2 v.a. ~ N(0,1)	*
	 * tendremos que correr las simulaciones solo N/2 veces 	*/
	count = 0;
	for (i=0 ; i<N/2 ; i++) {
		genZ (&Z);		/* Metimos dos N(0,1) en Z */
		if (Z.v1 < z)
			count++;
		if (Z.v2 < z)
			count++;
	}
	
/*	printf ("count = %d\n", count);
*/	
	printf ("Valor estimado: F(%.4f) = %.6f\n",
		y, (double) count / (double) N);
	
	return 0;
}
