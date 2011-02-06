#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"

#define CANT_ITERACIONES	1000


/* Funcion que calcula el M = {n : U1 <= U2 <= .. <= Un-1 > Un}.
 * Con P(M > n) = 1/fact(n).
*/
int calcular_M(void)
{
	double Ant = 0, Sig = 0.0;
	int Xn = 0;
	
	while (Ant <= Sig) {
		Ant = Sig;
		Sig = rg_gen_uni_cont(0,1);
		Xn += 1.0;
	}
	
	return Xn;
}
int main4(int argc, char * argv[])
{
	double Xobservada = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double result = 0;
	double mitadLongIntervalo = 0;
	int n = 0;
	
		
	for(n = 1; n <= CANT_ITERACIONES ; n++){
		Xobservada = calcular_M();
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
	}
	
	mitadLongIntervalo = 1.96 * sqrt(Sj1 / (double)n);

	printf("Valor exacto de E[X] = %f\n", exp(1.));
	printf("Valor estimado de E[X] = %f\n", Xpromj1);
	printf("La varianza muestral = %f\n", Sj1);
	printf("El intervalo de confianza [%f ; %f]\n", Xpromj1-mitadLongIntervalo,
		Xpromj1+mitadLongIntervalo);

	return 0;
}