#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"

double	TOL = 0.1;	/* Nivel de tolerancia deseado: S/sqrt(n) < TOL */





int main1 (void)
{
	double Normal = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double condition = 0;
	int n = 0;
	bool flag = false;
	
	/* Notar que S0 debe ser siempre = 0 para inicializar y que 
	 * Xpromj = X1 (osea el primer valor generado) */
	Xpromj = gen_normal_estandard_polar();
	
	while((n <= 30) || (sqrt(Sj1/((double)n)) >= TOL)) {
		Normal = gen_normal_estandard_polar();
		n++;
		Xpromj1 = estimar_media_muestral(n, Xpromj, Normal);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
		
	}
	
	printf("Se realizaron %d muestras\n", n);
	printf("Media muestral es %G\n", Xpromj1);
	printf("Varianza muestral es %G\n", Sj1);
	
	return 0;
}
