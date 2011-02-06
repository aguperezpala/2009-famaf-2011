#include "procesos_poisson.h"


int process_poisson_homogeneo(double T, double lambda, double *timesResults)
{
	int I = 0;
	double t = 0;
	double aux = 0;
	
	while (1){		
		aux = rg_gen_exp(lambda);
		if (t + aux > T)
			break;
		t = t + aux;
		I++;
		timesResults[I] = t;
	}
	
	return I;
}



int process_poisson_no_homogeneo(double T, func_ptr funLambda, double lambda, 
				 double *timesResults)
{
	int I = 0;
	double t = 0;
	double U = 0, V = 0, aux = 0;
	
	while (1){
		U = rg_gen_uni_cont(0,1);
		aux = rg_gen_exp(lambda);
		if (t + aux > T)
			break;
		t = t + aux;
		
		V = rg_gen_uni_cont(0,1);
		if(V < (funLambda(t)/lambda)) {
			I++;
			timesResults[I] = t;
		}
	}
	
	return I;
	
}

int process_poisson_no_homogeneo_mejorado(double *ts, int k, double *lambdas, 
				 double *timesResults, func_ptr funLambda)
{
	int I = 0;
	double t = 0;
	double U = 0, V = 0, aux = 0;
	int j = 0;
	double X = 0;
	
	while (1){
		U = rg_gen_uni_cont(0,1);
		X = rg_gen_exp(lambdas[j]);
		
		while(t+X > ts[j]) {
			if(j == k+1)
				return I;
			X = X -(ts[j] - t) * (lambdas[j]/lambdas[j+1]);
			t = ts[j];
			j++;
		}
		t += X;
		V = rg_gen_uni_cont(0,1);
		if(V < (funLambda(t)/lambdas[j])) {
			I++;
			timesResults[I] = t;
		}
	}
	
	return I;
}


