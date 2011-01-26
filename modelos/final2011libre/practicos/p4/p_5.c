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
#include "monte_carlo.h"


static double func_a(float x) {return pow (1.0-x*x, 1.5 );}

static double func_b(float x) {return x*pow(1+x*x,-2);}

static double func_c(float x) {return exp(-x*x);}

static double func_d(float x1, float x2)
{
	return (exp(pow(x1+x2, 2)));
}


int main5(void)
{
	int iter = 100;
	double result = 0;
	
	
	while (iter <= 1000000) {
		printf("\n\nIter: %d\n", iter);
		
		result = monte_carlo_normal(iter, func_a);
		printf("a) %G\n", result);
		
		result = monte_carlo_0toInf(iter, func_b);
		printf("b) %G\n", result);
		
		result = 2.0 * monte_carlo_0toInf(iter, func_c);
		printf("c) %G\n", result);
		
		result = monte_carlo_multiple(iter, func_d);
		printf("d) %G\n", result);
		
		iter *= 10;
	}
	
	
	return 0;
}