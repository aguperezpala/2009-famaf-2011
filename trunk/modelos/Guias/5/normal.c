/* Genera una v.a. ~ N(u,o) con el metodo de |Z| */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */
#define ITER	20	/* # de valores a generar de la v.a. */


struct timeval tv;	/* Para inicialización de ran2 */
static long idum = 0;


/* Genera un valor de |Z| para Z ~ N(0,1) */
static double gen_absZ (void)
{
	double	absZ = 0.0,	/* |Z| */
		U1   = 0.0,
		U2   = 0.0;
	static double	Y1   = 0.0,	/* Y1 ~ Exp(1) */
			Y2   = 0.0;	/* Y1 ~ Exp(1) */
	double t = 0.0;			/* t = Y1 + Y2 */
	static bool lazy = false;
	
	if (idum > 0) {
		/* Inicializamos ran2 */
		gettimeofday(&tv, NULL);
		idum = (long) -((tv.tv_sec << PAD) >> PAD);
		if (idum > 0)
			idum = -idum;
	}

	do {
/* Tradicional:
		if (!lazy) {
			U1 = ran2 (&idum);
			Y1 = - log (U1);
		}
		U2 = ran2 (&idum);
		Y2 = - log (U2);
		lazy = false;
 * Mejorada:                                                                  */
		if (!lazy) {
			/* Hay que generar ambas Y1 y Y2 */
			U1 = ran2 (&idum);
			U2 = ran2 (&idum);
			t  = - log (U1 * U2);	 /* t = Y1 + Y2 */
			U1 = ran2 (&idum);
			Y1 = t * U1;		 /* Pues Y1 ~ U(0,t) | Y1 + Y2 == t */
			Y2 = t - Y1;
		} else {
			/* Solo hay que generar Y2 */
			U2 = ran2 (&idum);
			Y2 = - log (U2);
			lazy = false;
		}
	} while ( Y2 < pow (Y1-1.0, 2.0) / 2.0 );
	
	absZ = Y1;
	Y1 = Y2 - (pow (Y1-1.0, 2.0) / 2.0);
	lazy = true;
	
	return absZ;
}


int main (int argc, char **argv)
{
	double	u = 0.0,	/* Media de la normal */
		s = 0.0,	/* Desv. est. de la normal */
		absZ = 0.0,	/* |Z| para Z ~ N(0,1) */
		N = 0.0,	/* V.A. ~ N(u,s) a generar */
		U = 0.0;	/* U(0,1), para sortear N */
	char *err=NULL;
	int i = 0;
	
	
	/* Manejo de la entrada */
	if (argc!=3) {
		fprintf (stderr, "Debe especificar los valores de u (media de "
				"la normal)\ny s (desv.est. de la normal), en "
				"ese orden, para generar una v.a. ~ N(u,s)\n");
		exit (EXIT_FAILURE);
	} else {
		/* Obtenemos el valor elegido para la media */
		u = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar T=\"límite superior de "
			"tiempo del proceso\" como primer parametro\n");
			exit (EXIT_FAILURE);
		}
		/* Obtenemos el valor elegido para la desviacion estandard */
		s = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar lambda=\"razon del proceso"
			"\" como segundo parametro\n");
			exit (EXIT_FAILURE);
		}
	}
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;	
	
	for (i=0 ; i<ITER ; i++) {
		
		absZ = gen_absZ();
		U = ran2(&idum);
		if (U < 0.5)
			N =  absZ;
		else
			N = -absZ;
		
		printf ("N = %.6f\n", N);
	}
	
	return 0;
}
