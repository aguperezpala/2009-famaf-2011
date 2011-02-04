#include "gen_continuas.h"


/******************************************************************************/
double rg_gen_uni_cont(double a, double b)
{
	double result = (double)mzran13()/(double)ULONG_MAX;
	
	result = result * (b-a) + a;
	
	return result;
}

/******************************************************************************/
double rg_gen_exp(double lambda)
{
	double U = 0;
	
	U = (double)mzran13()/(double)ULONG_MAX;
	
	return (((double) -1.0/ (double) lambda) * log(U));
}


/******************************************************************************/
double gen_gamma(int n, double lambda)
{
	int i = 0;
	double Uprod = 1;
	
	for(i = 0; i < n; i++)
		Uprod *= rg_gen_uni_cont(0,1);
	
	return (-1.0/lambda * log(Uprod));
}



/******************************************************************************/
int rg_gen_poisson(double lambda)
{
	int i = 0;
	double p = 0, F = 0;
	double U = 0;
	
	p = F = exp(-lambda);
	U = rg_gen_uni_cont(0,1);
	
	while(U >= F){
		p = p * lambda/(double)(i+1);
		F += p;
		i++;
	}
	
	return i;
}



/******************************************************************************/
double gen_normal_estandard(void)
{
	double y1 = 0., y2 = 0.;
	double lambda = 0.;
	double u = 0.;
	double z = 0.;
	
	lambda = 1.; /*Por teoria del metodo*/
	
	do {
		y1 = rg_gen_exp(lambda);
		y2 = rg_gen_exp(lambda);
	} while((y2 <= (pow((y1 - 1), 2) / 2.)));
	
	u = rg_gen_uni_cont(0,1);
	
	if(u <= 0.5){
		z = y1;
	}else{
		z = -y1;
	}
	
	return z;
}

double gen_normal_estandard_polar(void)
{
	double u = 0., v1 = 0., v2 = 0., s = 0., z = 0.;
	double X,Y;
	
	do {
		v1 = rg_gen_uni_cont(-1,1);
		v2 = rg_gen_uni_cont(-1,1);
		s = pow(v1,2) + pow(v2,2);
	}while(s >= 1);

	X = sqrt((-2. * log(s)) / s) * v1;
	Y = sqrt((-2. * log(s)) / s) * v2;
	
	/* perdemos eficiencia porque generamos 2... pero bue */
	
	return X;   
}
/******************************************************************************/
double gen_normal(double mu, double sigma)
{
	double z = 0.;
	double res = 0.;
	
	z = gen_normal_estandard_polar();
	
	res = (z * sigma) + mu;
	
	return res;
}


/******************************************************************************/




/******************************************************************************/


/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/*!*****************************************************************************/
double metodo_aceptacion_rechazo(funcp_1par_t f, funcp_1par_t g, double c,
				 funcp_0par_t genY)
{
	double U = 0;
	double Y = 0;
	
	do {
		Y = genY();
		U = rg_gen_uni_cont(0,1);
	} while (U >= f(Y)/(g(Y)*c));
	
	return Y;
}

