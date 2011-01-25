#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "rdg.h"
#include "gen_continuas.h"


/* La aguja del buffon, tenemos los siguientes datos:
*  	t la distancia entre las rectas.
*	l la longitud de la aguja.
*	θ la medida del ángulo agudo entre la aguja (o su prolongación) y una de las rectas.
*	x la distancia entre el punto medio de la aguja y la recta más cercana
* x y θ son v.a. uniformes con distribución f (x) y g(θ):
* f(x) = 2/t ;  g(θ) = 2/π
* Tenemos entonces que una aguja corta una recta si y solo si
* x < (l/2) * sen(θ) 
* con P(aguja corte la recta) = (2*l) / (π * t).
*/


int main5(void)
{
	double total = 0;
	int counter = 0, iter = 1000000, i = 0;
	double centro = 0, angulo = 0, ya = 0, yb = 0;
	
	
	for(i = 0; i < iter; i++) {
		/* calculamos el centro y el angulo de caida de la aguja, la distancia
		 * entre las lineas es 2. La otra variable aleatoria (angulo)
		 * va de 0 <= angulo <= PI*/
		centro = rg_gen_uni_cont(0.0, 2.0);
		angulo = rg_gen_uni_cont(0.0, M_PI);
		
		ya = centro + 0.5*cos(angulo);
		yb = centro - 0.5*cos(angulo);
		if (ya<0.0 || ya>2.0 || yb<0.0 || yb>2.0)
			counter++;
	}
	
	/* Numero total de lanzamientos / numero de veces que la aguja interseco una
	 * de las lineas */
	total = (double)iter / (double)counter;
	
	printf("Promedio total: %G\n", total);
	
	return 0;
}