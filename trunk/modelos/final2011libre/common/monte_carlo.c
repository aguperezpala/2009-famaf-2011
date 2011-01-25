#include "monte_carlo.h"


/******************************************************************************/
double monte_carlo_normal(int k, FUNC_PTR g)
{
	int i = 0;
	double result = 0;
	double U = 0;
	
	for(i = 0; i < k; i++) {
		U = rg_gen_uni_cont(0.0,1.0);
		result += g(U);
	}
	
	return (result / (double)k);
	
}

/******************************************************************************/
double monte_carlo_ab(int k, FUNC_PTR g, double a, double b)
{
	int i = 0;
	double result = 0;
	double U = 0;
	double fac = b-a;
	
	for(i = 0; i < k; i++) {
		U = rg_gen_uni_cont(0.0,1.0);
		result += g(a+ fac * U) * fac;
	}
	
	return (result / (double)k);	
}

/******************************************************************************/
double monte_carlo_0toInf(int k, FUNC_PTR g)
{
	int i = 0;
	double result = 0;
	double U = 0;
	
	for(i = 0; i < k; i++) {
		U = rg_gen_uni_cont(0.0,1.0);
		result += g((1.0/U)-1.0) / pow(U,2.0);
	}
	
	return (result / (double)k);	
}

/******************************************************************************/
double monte_carlo_multiple(int k, FUNC_PTR_COMP g)
{
	int i = 0, j = 0;
	double result = 0;
	double U[MONTE_CARLO_COMP_N] = {};
	
	for(i = 0; i < k; i++) {
		/* generamos las Ui */
		for(j = 0; j < MONTE_CARLO_COMP_N; j++) {
			U[j] = rg_gen_uni_cont(0.0,1.0);
		}
		/* TODO: MODIFICAR esto si la g toma mas de 2 parametros */
		result += g(U[0], U[1]);
	}
	
	return (result / (double)k);
}



