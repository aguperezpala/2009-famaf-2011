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

#define PAD 40	/* Definido para arquitectura de 64 bits */



static double func_d(float x1, float x2)
{
	return (exp(pow(x1+x2, 2)));
}

static double f1(float x2)
{	
	
	return pow (1.0-x2*x2, 1.5 );
	//return pow((double)1 - pow(x,2), ((double)3)/((double)2));
}


int main(void)
{
	double montecarlo1, montecarlo2;
	/*
	montecarlo1 = monte_carlo_normal(99999, f1);
	montecarlo2 = monte_carlo_ab(99999, f1, 0, 1);
	*/
	
	montecarlo1 = monte_carlo_multiple(99999999, func_d);
	
	printf("montecarlo1: %G\nmontecarlo1: %G\n", montecarlo1, montecarlo2);
	return 0;
}