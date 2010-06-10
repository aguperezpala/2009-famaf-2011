#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"
#include "ssv.h"

#define PAD	40	/* Definido para arquitectura de 64 bits */

unsigned int SIM = 6147;	/* # de simulaciones a realizar */
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
	unsigned int n = 0, success = 0;
	double x = 0.0, y = 0.0;
	
	/* Variables relacionadas con los resultados de las simulaciones */
	double	p = 0.0,	/* estimador: fracción muestral */
		dep = 0.0,	/* desviación estándard de p */
		l = 0.0,u = 0.0;/* extremos del IC */
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;

	/* Ciclo principal de simulacion */
	success = 0;
	for (n=0 ; n<SIM ; n++) {
		/* Tiramos el punto en el cuadrado */
		x = ((ran2 (&idum)) * 2.0) - 1.0;
		y = ((ran2 (&idum)) * 2.0) - 1.0;
		/* Lo contamos sii cayó dentro de la circunferencia */
		if (sqrt (x*x + y*y) < 1.0)
			success++;
	}
	p = (double) success / (double) n;
	dep = sqrt (p * (1.0-p) / (double) n);
	l = 4.0 * p - 4.0 * za * dep;
	u = 4.0 * p + 4.0 * za * dep;
	
	/* Informe de los resultados */
	printf ("Se realizaron %d simulaciones del problema\n\n"
		"La fracción muestral fue:    p = %.8f\n"
		"Y su desviación estándard: dep = %.8f\n\n"
		"El intervalo [ %.4f , %.4f ] contiene el valor de la cte. PI\n"
		"con un nivel de confianza del 95%%\n",
		SIM, p, dep, l, u);

	return 0;
}
