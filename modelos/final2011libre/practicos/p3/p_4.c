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

/* Tenemos que calcular N = Maximo{n: Productoria Ui >= exp(-3) from i=1 to n}
 * Para la parte a) tenemos que determinar E[N]
 * Para la parte b) tenemos que determinar P(N = i) para i = 0..6, usando 
 * n = 1000000, osea, lo que hacemos es correr para cada i, 1000000 veces y ver
 * cuantas veces coincidio que el contador = i. luego la P(N=i) = counter/1000000
 */

static void p_4_parte_a(void)
{
	double result = 1, total = 0;
	int counter = 0, iter = 1000000, i = 0;
	double e = pow (M_E, -3.0);
	
	for(i = 0; i < iter; i++) {
		while (result >= e) {
			result *= rg_gen_uni_cont(0.0, 1.0);
			counter++;
		}
		/* restamos uno porque si salimos del bucle quiere decir que
		* el maximo n era el anterior antes de salir tal que cumplia
		* con la guarda */
		total += counter-1;
		counter = 0;
		result = 1;
	}
	total = total / (double)iter;
	
	printf("Promedio total: %G\n", total);	
}


static void p_4_parte_b(void)
{
	double result = 1, total = 0;
	int counter = 0, iter = 1000000, i = 0, j = 0;
	int exitos = 0;
	double e = pow (M_E, -3.0);
	
	for(i = 0; i < 7; i++) {
		exitos = 0;
		
		for(j = 0; j < iter; j++) {
			while (result >= e) {
				result *= rg_gen_uni_cont(0.0, 1.0);
				counter++;
			}
			counter--;
			if(counter == i) {
				exitos++;
			}
			counter = 0;
			result = 1;
		}
		total = (double)exitos / (double) iter;
		printf("P(N = %d) = %G\n", i, total);
	}
}



int main4(void)
{
	p_4_parte_a();
	p_4_parte_b();
	
	return 0;
}