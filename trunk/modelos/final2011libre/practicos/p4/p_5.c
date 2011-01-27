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

#define LAMBDA		2.71828182845904523536028
#define K_FUNCION	15
#define NUM_ITER	100


static double constante = 0;

static double fact (unsigned long j)
{
	unsigned long i=j, res=1;
	for (i=j ; i>0 ; i--)
		res *= i;
	return (double) res;
}

static double sumatoria_auxiliar(double lda, int k)
{
	unsigned int i=0;
	double res=0.0, fact[2]={1.0,1.0};
	res += pow (lda, 0.0) / fact[0];
	
	for (i=1 ; i<=k ; i++) {
		fact[i%2] = fact[(i+1)%2] * i;
		res += pow (lda, (double) i) / fact[i%2];
	}
	return res;
}

static double func_distrib(int i)
{
	double result = 1.0 / constante;
	int k = 1;
	
	for(k = 1; k <= i; k++){
		result *= LAMBDA / (double) k;
	}
	
	return result;
}

/* funcion usada para los qj, usamos uniforme */
double distrib_uniforme(int i, int n){
	/* No usamos i pero lo dejamos por conveniencia, y tenemos que el tamanio
	 * va de 0 a n, asique debemos devolver 1/(n+1) */
	return (1./(double)(n+1));
}

int main5(void)
{
	int iter = 0;
	double transfInversa = 0, ti = 0;
	double acceptRechazo = 0, ar = 0;
	double mu = floor(LAMBDA);
	
	constante = sumatoria_auxiliar(LAMBDA, K_FUNCION);
	
	double cota = (pow(LAMBDA, mu)/fact(mu))*((K_FUNCION+1)/constante);
	
	printf("constante: %f\n", 1./constante);
	
	for(iter = 0; iter < NUM_ITER; iter++){
		ti = disc_trasnf_inversa_func(func_distrib);
		transfInversa += ti;
		ar = disc_aceptacion_rechazo(func_distrib, distrib_uniforme, 
							cota, K_FUNCION);		
		acceptRechazo += ar;
		
		printf("transfInversa: %f\tacceptRechazo: %f\n", ti, ar);
	}
	
	transfInversa /= (double) NUM_ITER;
	acceptRechazo /= (double) NUM_ITER;
	
	printf("E[transfInversa]: %f\tE[acceptRechazo]: %f\n", transfInversa,
	       acceptRechazo);
	
	
	return 0;
}