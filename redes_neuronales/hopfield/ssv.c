#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>
#include <sys/time.h>
#include "ssv.h"

#define SIZE 2


/** Estructura Media muestral */
double	media[SIZE] = {0.0, 0.0};
unsigned int	ma = 0,  /* media[ma] = media anterior  = X(n-1) */
		ms = 0;  /* media[ms] = media siguiente = X(n)   */

/** Estructura Varianza muestral */
double	var[SIZE] = {0.0, 0.0};		/* Varianza muestral */
unsigned int	va = 0,  /* var[va] = varianza anterior  = S^2(n-1) */
		vs = 0;  /* var[vs] = varianza siguiente = S^2(n)   */



/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la media muestral = X(n)
 */
void
media_m (double Xn, double n)
{
	/* Recordar:	media[ma] = media anterior  = X(n-1) *
	 *		media[ms] = media siguiente = X(n)   *
	 *						     */
	ma = ms;
	ms = (ms+1) % SIZE;
	media[ms] = media[ma] + (Xn-media[ma]) / n;
	
	return;
}


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la varianza muestral = S^2(n)
 *
 * NOTE: ASUME que (media[ma] , media[ms]) tienen los valores (X(n-1) , X(n))
 * 	 respectivamente (ver arriba la funcion media_m, "Media muestral")
 */
void
var_m (double Xn, double n)
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
	
	return;
}


/* Devuelve la última media muestral calculada */
double
get_media_m (void) { return media[ms]; }


/* Devuelve la última varianza muestral calculada */
double
get_var_m (void) { return var[vs]; }


/* Reinicializa la media, como si volviesemos al paso 0 */
void
reset_media_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		media[i] = 0.0;
	return;
}


/* Reinicializa la varianza, como si volviesemos al paso 0 */
void
reset_var_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		var[i] = 0.0;
	return;
}

