#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include "rdg.h"


#define PAD	40	/* Definido para arquitectura de 64 bits */
#define SIZE 2

double	TOL = 0.01;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */

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
	
	/* El 1er paso lo realizamos afuera del ciclo general	*
	 * por issues con la generacion de la varianza muestral */
	
	U = ran2(&idum);
	Xn = f(U);
	est += Xn;
	
	n = 1.0;
	X = media_m (Xn, n);
	
	/* Ciclo principal de simulacion */
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
