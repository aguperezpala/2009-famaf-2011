#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */
#define SIZE 2

double	TOL = 0.1;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */

/** Media muestral */
double	media[SIZE] = {0.0, 0.0};
unsigned int	ma = 0,  /* media[ma] = media anterior  = X(n-1) */
		ms = 0;  /* media[ms] = media siguiente = X(n)   */

/** Varianza muestral */
double	var[SIZE] = {0.0, 0.0};		/* Varianza muestral */
unsigned int	va = 0,  /* var[va] = varianza anterior  = S^2(n-1) */
		vs = 0;  /* var[vs] = varianza siguiente = S^2(n)   */


/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la media muestral = X(n)
 */
static double media_m (double Xn, double n)
{
	/* Recordar:	media[ma] = media anterior  = X(n-1) *
	 *		media[ms] = media siguiente = X(n)   *
	 *						     */
	
	ma = ms;
	ms = (ms+1) % SIZE;
	media[ms] = media[ma] + (Xn-media[ma]) / n;
	
	return media[ms];
}


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la varianza muestral = S^2(n)
 *
 * NOTE: ASUME que (media[ma] , media[ms]) tienen los valores (X(n-1) , X(n))
 * 	 respectivamente (ver arriba la funcion mm, "Media muestral")
 */
static double var_m (double Xn, double n)
{
	/* Recordar:	var[va] = varianza anterior  = S^2(n-1) *
	 *		var[vs] = varianza siguiente = S^2(n)	*
	 *							*/

	va = vs;
	vs = (vs+1) % SIZE;
	var[vs] = ((n - 1.0) / n) * var[va] + \
		   (n + 1.0) * pow (media[ms] - media[ma], 2.0);
	
	return var[vs];
}




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
	
	
	/* El 1er paso lo realizamos afuera del ciclo general	*
	 * por issues con la generacion de la varianza muestral */
	
	/* Generamos 2 va: X1,X2 ~ N(0,1) */
	do {
		V1 = ((ran2 (&idum)) * 2.0) - 1.0;
		V2 = ((ran2 (&idum)) * 2.0) - 1.0;
		/* V1,V2 ~ U(-1,1) */
		C = V1*V1 + V2*V2;
	} while (C > 1.0);
	
	X1 = sqrt (-2.0 * log(C) / C) * V1;
	X2 = sqrt (-2.0 * log(C) / C) * V2;
	
	n = 1.0;
	X = media_m (X1, n);
	n += 1.0;
	X = media_m (X2, n);
	S = sqrt (var_m (X2, n));
	
	/* Ciclo principal de simulacion */
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
