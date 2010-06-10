#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include "rdg.h"
#include "ssv.h"

#define PAD	40	/* Definido para arquitectura de 64 bits */

unsigned int SIM = 1000;	/* # de simulaciones a realizar */
double za = 1.96;		/* nivel de confianza deseado para el IC:
				 * 1.64  =>  90 %
				 * 1.96  =>  95 %
				 * 2.33  =>  99 %
				 */

int main (void)
{
	/* Inicializacion de ran2 */
	struct timeval tv;
	long idum = 0;
	
	/* Variables relacionadas con las simulaciones */
	unsigned int n = 0;
	double	N = 0.0, Xn = 0.0, est = 0.0;
	
	/* Variables relacionadas con los resultados de las simulaciones */
	double	X = 0.0,	/* media muestral en el paso 'n' */
		S = 0.0,	/* desv. est. muestral en 'n' */
		l = 0.0,u = 0.0;/* extremos del IC */
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	/* Ciclo principal de simulacion */
	est = 0.0;
	for (n=0 ; n<SIM ; n++) {
		
		N = 0.0;
		Xn = 0.0;
		while (N < 1.0) {
			N += ran2 (&idum);
			Xn += 1.0;
		}
		est += Xn;
		
		X = media_m (Xn, (double) n+1);
		S = sqrt (var_m (Xn, (double) n+1));
	}
	
	l = (est / (double) n) - za * (S / sqrt((double) n));
	u = (est / (double) n) + za * (S / sqrt((double) n));
	
	/* Informe de los resultados */
	printf ("Se realizaron %d simulaciones del problema\n\n"
		"La media muestral del estimador es:      X = %.8f\n"
		"La varianza muestral del estimador es: S^2 = %.8f\n\n"
		"El intervalo [ %.4f , %.4f ] contiene al valor esperado\n"
		"de 'N' con una confianza del 95%%\n",
		SIM, X, S*S, l, u);
	
	printf ("\nDato curioso sobre la precisión de los métodos estimativos:"
		"\nX(n) = %.20f\test/n = %.20f\n\n", X, est / (double) n);
	
	return 0;
}
