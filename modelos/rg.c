#include "rg.h"

typedef unsigned long int unlong;

static unlong mzran13(void)
{ 
	long int s;
	static unlong x=521288629, y=362436069, z=16163801, c=1, n=1131199209;
	if (y>x+c) {
		s=y-(x+c); 
		c=0;
	} else { 
		s=y-(x+c)-18;
		c=1;
	}
	x=y;
	y=z;
	return (z=s) + (n=69069*n+1013904243);
}

/* Funcion que genera una variable aleatoria de Poisson con parametro
* lambda */
int rg_gen_poisson(double lambda)
{
	int i = 0;
	double p = 0, F = 0;
	double U = 0;
	
	p = F = exp(-lambda);
	U = ((double)mzran13()/(double)ULONG_MAX);
	
	while(U >= F){
		p = p * lambda/(double)(i+1);
		F += p;
		i++;
	}
	
	return i;
}

/* generadora de una v.a. exponencial con parametro lambda */
double rg_gen_exp(double lambda)
{
	double U = 0;
	
	U = (double)mzran13()/(double)ULONG_MAX;
	
	return (((double) -1.0/ (double) lambda) * log(U));
}

/* funcion que genera una v.a. uniforme discreta en el intervalo [a,b] */
int rg_gen_uni_dis(int a, int b)
{
	double result = (double)mzran13()/(double)ULONG_MAX;
	
	
	return ((int)((result * (b-a))/1) + a);
}

/* funcion que genera una v.a. uniforme continua en (a,b) */
double rg_gen_uni_cont(double a, double b)
{
	double result = (double)mzran13()/(double)ULONG_MAX;
	
	result = result * (b-a) + a;
	
	return result;
}
