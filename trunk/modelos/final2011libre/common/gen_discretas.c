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


