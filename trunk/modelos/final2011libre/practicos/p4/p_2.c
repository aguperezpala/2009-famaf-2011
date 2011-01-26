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


double func_exacta()
{
	int k = 0;
	int N = 10000;
	double result = 0;
	
	for(k = 1; k <= N; k++){
		result += exp((double)k/(double)N);
	}
	
	return result;
}


double func_a_estimar(double x)
{	
	return (exp(x/(double)10000.0));
}


/* Mas facil hacerlo todo aca porque esta andando mal...
double f(double x, double n){      return(exp(x/n));      }

int main(int argc, char *argv[]){
       double suma = 0.;
       double u = 0.;
       int cant_it = 0; // Cantidad de iteraciones
       int i = 0;
       
      cant_it = 1000;
       
      // calculamos la aproximacion
       for(i=0; i<cant_it; i++){
       u = floor(uniforme_0_1()*(double)N)+1;
       suma += f(u, (double)N);
       }
       
       suma *= cant_it;
       
       printf("La aproximacion es: %f\n", suma);
       
       // el valor verdadero seria
       cant_it = N;
       suma = 0;
       for(i=1; i<=cant_it; i++){       
       suma += f(i, (double)N);
       }
       
       
       return 0;
}
*/
       



int main2(void)
{
	double valReal = 0;
	double valEstimado = 0;
	
	
	valEstimado = calc_prom(10000, 1000, &func_a_estimar);	
	valReal = func_exacta();
	
	
	/*int i = 0;
	double U = 0;
	for(i = 0; i < 1000; i++) {
		U = disc_gen_uniforme(1,10000);
		//U = floor(((1./(float)ULONG_MAX)*mzran13())*10000.0)+1;
		valEstimado += exp(U/10000.0);
	}
	valEstimado *= 1000;
	*/
	
	printf("Valor real: %f\n", valReal);
	printf("Valor Estimado: %f\n", valEstimado);
	
	return 0;
}