#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"
#include "ssv.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */

double	TOL = 0.1;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */





int main (void)
{
	/* Inicializacion de ran2 */
	struct timeval tv;
	long idum = 0;
	
	/* Simulacion (gen. de muestras) */
	double	V1 = 0.0, V2 = 0.0,	/* Coordenadas del punto en el circulo */
		X1 = 0.0, X2 = 0.0,	/* Variables aleatorias ~ N(0,1) */
		C = 0.0;		/* V1^2 + V2^2 */
	
	/* Variables relacionadas con las muestras */
	double	n = 0.0,		/* # de intento, paso en el que vamos */
		X = 0.0,		/* media muestral en el paso 'n' */
		S = 0.0;		/* desv. est. muestral en 'n' */
	
	
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	/* Ciclo principal de simulacion */
	n = 0.0;
	while (S/sqrt(n) >= TOL || n < 30.0) {
		
		/* Generamos 2 va: X1,X2 ~ N(0,1) */
		do {
			V1 = ((ran2 (&idum)) * 2.0) - 1.0;
			V2 = ((ran2 (&idum)) * 2.0) - 1.0;
			/* V1,V2 ~ U(-1,1) */
			C = V1*V1 + V2*V2;
		} while (C > 1.0);
		
		X1 = sqrt (-2.0 * log(C) / C) * V1;
		X2 = sqrt (-2.0 * log(C) / C) * V2;
		
		n += 1.0;
		X = media_m (X1, n);
		S = sqrt (var_m (X1, n));
		
		if (S/sqrt(n) < TOL)
			/* Ya terminamos, no nos hizo falta X2 */
			continue;
		
		n += 1.0;
		X = media_m (X2, n);
		S = sqrt (var_m (X2, n));
	}
	
	/* Informe de los resultados */
	printf ("\nSe realizaron %.0f generaciones de datos\n"
		"La media muestral obtenida es:   X = %.8f\n"
		"La varianza muestral fue:      S^2 = %.8f\n\n",
		floor(n), X, S*S);
	
	return 0;
}
