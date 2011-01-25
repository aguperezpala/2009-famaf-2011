#include "gen_discretas.h"


/******************************************************************************/
int disc_transf_inversa(double *pi, int size)
{
	double F = pi[0];
	double U = rg_gen_uni_cont(0.0,1.0);
	int i = 0;
	
	while(F <= U){
		i++;
		F += pi[i];
	}
	
	return i;
}

/******************************************************************************/
int disc_gen_uniforme(int a, int b)
{
	int result = floor(rg_gen_uni_cont(0.0,1.0)) * (b-a) + a;
	
}

/******************************************************************************/
int disc_gen_geometrica(double p)
{	
	double U = rg_gen_uni_cont(0.0,1.0);
	
	return (int)(floor(log(U) / log(1-p)) + 1);
}

/******************************************************************************/
int disc_gen_bin_negativa(int r, double p)
{
	int X = 0, Y = 0;
	int i = 0;
	double U = 0;
	
	for(i = 0; i < r; i++) {
		U = rg_gen_uni_cont(0.0,1.0);
		Y = (int)(floor(log(U) / log(1-p)) + 1);
		
		X += Y;
	}
	
	return X;
}

/******************************************************************************/
int disc_gen_poisson(double lambda)
{
	int i = 0;
	double p = 0, F = 0;
	double U = 0;
	
	p = F = exp(-lambda);
	U = rg_gen_uni_cont(0.0,1.0);
	
	while(U >= F){
		p = p * lambda/(double)(i+1);
		F += p;
		i++;
	}
	
	return i;
}