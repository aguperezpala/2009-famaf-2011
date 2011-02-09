#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "analisis_estadistico.h"
#include "gen_discretas.h"



#define TAM_MUESTRA	10
#define LAMBDA_EXP	1	

/* # de simulaciones a ejecutar para ajustar el p-valor */
#define NSIM  10000

static double F_exponencial(double x)
{
	return (1.0-exp(-LAMBDA_EXP*x));
}


int main (void)
{
	double p_value = 0.0;
	unsigned int i = 0, j = 0;
	double Muestra[TAM_MUESTRA];
	double MuestraSim[TAM_MUESTRA];
	double Dsim = 0.0;
	int counter = 0;
	double Dvalor = 0.0;
	
	
	/* generamos la muestra inicial */
	for(i = 0; i < TAM_MUESTRA; i++){
		Muestra[i] = rg_gen_exp(LAMBDA_EXP);
	}
	
	Dvalor = kolmogorov_smirnov (Muestra, TAM_MUESTRA, F_exponencial);
	
	/* ahora simulamos */
	for(i = 0; i < NSIM; i++) {
		/* generamos la muestra sim */
		for(j = 0; j < TAM_MUESTRA; j++){
			MuestraSim[j] = rg_gen_exp(LAMBDA_EXP);
		}
		Dsim = kolmogorov_smirnov (MuestraSim, TAM_MUESTRA, F_exponencial);
		
		if(Dsim >= Dvalor)
			counter++;
	}
	
	Dsim = (double)counter/(double)NSIM;
	
	printf("El Dvalor de K-S es: %.8f\n", Dvalor);
	printf("El p-valor simulado de K-S es: %.8f\n", Dsim);
	
	return 0;
}
