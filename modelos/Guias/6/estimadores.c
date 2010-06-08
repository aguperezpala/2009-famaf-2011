#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include "rdg.h"

#define SIZE 2
double media[SIZE] = {0.0, 0.0};
double var[SIZE] = {0.0, 0.0};

/* Media muestral para el paso 'n'
 * Devuelve la posicion a la que debe ser accedida la variable global "media"
 * para obtener el proximo valor de la media muestral = X(n)
 */
unsigned int mm (double Xn, int n)
{
	static unsigned int i = 0, /* X(n)   = media[i] */
			    j = 0; /* X(n-1) = media[j] */
	
	j = i;
	i = (i+1) % SIZE;
	media[i] = media[j] + (Xn-media0) / (double) n;
	
	return i;
}


/* Varianza muestral para el paso 'n'
 * Devuelve la posicion a la que debe ser accedida la variable global "var"
 * para obtener el proximo valor de la varianza muestral = S^2(n)
 */
unsigned int vm (double)