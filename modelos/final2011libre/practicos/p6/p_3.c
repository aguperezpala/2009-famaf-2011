#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"


#define CANT_ITERACIONES	1000


/*! tenemos que calcular N = Minimo{n : Sumatoria Ui > 1 from i=1 to n} */
int calcular_N(void)
{
	double suma = 0, u = 0;
	int n = 0;
	
	while (suma <= 1){
		u = rg_gen_uni_cont(0,1);
		suma += u;
		n++;
	}
	return n;
}


int main3(int argc, char * argv[]){
	double Xobservada = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double result = 0;
	double mitadLongIntervalo = 0;
	int n = 0;
	

	
	for(n = 1; n <= CANT_ITERACIONES ; n++){
		Xobservada = calcular_N();
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
	}
	
	/* calculamos el intervalo de N con 95% de confianza => 1-alfa = 0.95
	 * => Zalfa/2 = 1.96. Luego el intervalo de confianza es
	 * EstimadorN = N| +- 1.96 * sqrt(Sj1/(double)n);
	 */
	
	mitadLongIntervalo = 1.96 * sqrt(Sj1 / (double)n);
	
	
	printf("Valor exacto de E[X] = %f\n", exp(1.));
	printf("Valor estimado de E[X] = %f\n", Xpromj1);
	printf("La varianza muestral = %f\n", Sj1);
	printf("El intervalo de confianza [%f ; %f]\n", Xpromj1-mitadLongIntervalo,
	       Xpromj1+mitadLongIntervalo);

	return 0;
}