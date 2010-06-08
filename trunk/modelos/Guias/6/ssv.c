#include <math.h>
#include "rdg.h"
#include "ssv.h"

#define SIZE 2


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
double media_m (double Xn, double n)
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
 * 	 respectivamente (ver arriba la funcion media_m, "Media muestral")
 */
double var_m (double Xn, double n)
{
	/* Recordar:	var[va] = varianza anterior  = S^2(n-1) *
	 *		var[vs] = varianza siguiente = S^2(n)	*
	 *							*/
	if (n > 1) {
		va = vs;
		vs = (vs+1) % SIZE;
		var[vs] = ((n - 1.0) / n) * var[va] + \
			(n + 1.0) * pow (media[ms] - media[ma], 2.0);
	}
	
	return var[vs];
}


