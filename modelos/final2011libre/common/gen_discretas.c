#include "gen_discretas.h"


/******************************************************************************/
int disc_transf_inversa(double *pi)
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
int disc_trasnf_inversa_func2(FUNC_TI_PTR funcDistr, int j)
{
	double F = funcDistr(j);
	double U = rg_gen_uni_cont(0.0,1.0);
	int i = j;
	
	while(F <= U){
		
		i++;
		F += funcDistr(i);
	}
	
	return i;
	
}
int disc_trasnf_inversa_func(FUNC_TI_PTR funcDistr)
{
	double F = funcDistr(0);
	double U = rg_gen_uni_cont(0.0,1.0);
	int i = 0;
	
	while(F <= U){
	
		i++;
		F += funcDistr(i);
	}
	
	return i;
	
}

/******************************************************************************/
int disc_gen_uniforme(int a, int b)
{
	int result = floor(rg_gen_uni_cont(0.0,1.0) * (b-a)) + a;
	return result;
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

/******************************************************************************/
int disc_gen_binomial(int n, double p)
{
	double U = rg_gen_uni_cont(0.0,1.0);
	int i = 0;
	double c = (double)p/(double)(1-p);
	double Pr = pow(1-p, n);
	double F = Pr;
	
	while (U >= F) {
		Pr = (((double) c*(n-i)) / (double)(i+1)) * Pr;
		F += Pr;
		i++;
	}
	return i;
}



/*!*****************************************************************************/



/*****************************************************************************/
int disc_aceptacion_rechazo(double (*funcDistr_Pj)(int), double (*funcDistrRechazo_Qj)(int,int), 
			    double cota, int k)
{
	double U1 = 0., U2 = 0., Y = 0.;
		
	do {
		U1 = rg_gen_uni_cont(0.0,1.0);
		Y = disc_gen_uniforme(0, k+2);	/* generamos entre 0 y k+1 (es abierto el intervalo) */
		U2 = rg_gen_uni_cont(0.0,1.0);
	} while(U2 >= ((funcDistr_Pj)(Y)/(cota * (funcDistrRechazo_Qj)(Y,k))));
	
	return Y;
}



/*!*****************************************************************************/

/*****************************************************************************/
int disc_metodo_composicion(double (*P1j)(int), double (*P2j)(int), double alfa, int j)
{
	double U = rg_gen_uni_cont(0.0,1.0);
	
	if (U < alfa){
		return disc_trasnf_inversa_func2(P1j,j); 
	} else {
		return disc_trasnf_inversa_func2(P2j,j);
	}
}

















