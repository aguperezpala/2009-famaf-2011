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

/* Notar que N = min { n: Sum Ui > 1.0 from i=1 to n} tenemos que 
 * la funcion de densidad de masa P(j) = (j-1)/j!
 * E[N] = Sum (1/(j-1)!) j=1 to inf == e    (exp).
*/


int main(void)
{
	double result = 0, total = 0;
	int counter = 0, iter = 1000000, i = 0;
	
	for(i = 0; i < iter; i++) {
		while (result <= 1.0) {
			result += rg_gen_uni_cont(0.0, 1.0);
			counter++;
		}
		total += counter;
		counter = 0;
		result = 0;
	}
	total = total / (double)iter;
	
	printf("Promedio total: %G\n", total);
	
	return 0;
}