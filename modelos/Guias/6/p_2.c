#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"
#include "ssv.h"

#define PAD	40	/* Definido para arquitectura de 64 bits */

double	TOL = 0.01;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */


static double f (double x) { return exp (x*x); }


int main (void)
{
	/* Inicializacion de ran2 */
	struct timeval tv;
	long idum = 0;
	
	/* Simulacion */
	double	  U = 0.0,	/* v.a. ~ U(0,1) */
		 Xn = 0.0,	/* punto de la integral */
		est = 0.0;	/* valor estimado para la integral */
	
	/* Variables relacionadas con las muestras */
	double	n = 0.0,	/* # de intento, paso en el que vamos */
		X = 0.0,	/* media muestral en el paso 'n' */
		S = 0.0;	/* desv. est. muestral en 'n' */
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	/* Ciclo principal de simulacion */
	n = 0.0;
	while (n < 100.0 || S/sqrt(n) > TOL) {
		
		/* Generamos un nuevo dato y lo acumulamos */
		U  = ran2 (&idum);
		Xn = f(U);
		est += Xn;
		
		/* Actualizamos nuestras variables aleatorias */
		n += 1.0;
		X = media_m (Xn, n);
		S = sqrt (var_m (Xn, n));
		
		if (((int) n % 1000000) == 0)
			printf ("Loop # %.0fx10^6\t\tS = %.4f\n", n/1000000, S);
	}
	
	/* Informe de los resultados */
	printf ("\nSe realizaron %.0f generaciones de datos\n"
		"La media muestral obtenida es:   X = %.8f\n"
		"La varianza muestral fue:      S^2 = %.8f\n"
		"El valor estimado de la integral es: %.8f\n\n",
		floor(n), X, S*S, est/n);
		
	return 0;
}
