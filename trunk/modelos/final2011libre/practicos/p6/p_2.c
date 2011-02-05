#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"

static double	TOL = 0.01;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */

/*! Para este ejercicio tenemos que estimar la integral de una funcion por
 * medio de montecarlo. Luego nuestro estimador I = integrate f(x)dx from 0 to 1
 * donde f(x) = exp(pow(x,2)).
 * usamos entonces en cada ciclo una estimacion por montecarlo haciendo
 * f(U) con U ~ Uniforme(0,1) en cada ciclo (eso nos devuelve cada Xi observado).
 * y en base a eso calculamos la Varianza muestral y la media muestral
 */

static double montecarlo_f(double x) {return exp(pow(x,2));}


int main (void)
{
	double Xobservada = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double condition = 0;
	double ValorIntegral = 0;
	int n = 0;
	bool flag = false;
	
	/* Notar que S0 debe ser siempre = 0 para inicializar y que 
	 * Xpromj = X1 (osea el primer valor generado) */
	Xpromj = montecarlo_f(rg_gen_uni_cont(0,1));
	ValorIntegral = Xpromj;
	while((n <= 100) || (sqrt(Sj1/((double)n)) >= TOL)) {
		Xobservada = montecarlo_f(rg_gen_uni_cont(0,1));
		ValorIntegral += Xobservada;
		n++;
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
		
	}
	
	ValorIntegral = ValorIntegral / (double)n;
	
	printf("Se realizaron %d muestras\n", n);
	printf("Media muestral es %G\n", Xpromj1);
	printf("Varianza muestral es %G\n", Sj1);
	printf("Valor integral es %G\n", ValorIntegral);
	
	return 0;
}
