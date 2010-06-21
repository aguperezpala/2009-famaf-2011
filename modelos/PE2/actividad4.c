#include "actividad4.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>


/* Funcion que genera el valor de la integral en un intervalo entre a y b
 * y la funcion fun.
 */
static double act4_monte_carlo(double a, double b, double (*fun)(double))
{
	double U = 0;
	double acum = 0;
	int i = 0;
	//MONTECARLO_BOUND
	
	for(i = 0; i < MONTECARLO_BOUND; i++) {
		U = mzran13()/(double)ULONG_MAX;
		U = (U * (b-a)) + a;
		
		acum += fun(U);
	}
	
	return (acum/(double)MONTECARLO_BOUND);
}

/* Funcion que genera intervalos.
* REQUIRES:
* 	begin 		comienzo del intervalo
*	end		fin del intervalo
*	n		cantidad de intervalos
*	I		arreglo donde se va a llenar
* RETURNS:
* 	< 0		on error
* 	0		if success
*/
int act4_gen_intervals(double begin, double end, int n, double *I)
{
	double delta = 0;
	int i = 0;
	
	assert(I != NULL);
	delta = (end-begin)/(double)n;
	
	if(delta <= 0)
		return -1;
	
	I[0] = begin;
	for(i = 1; i < n; i++)
		I[i] = begin + ((double)i) * delta;
	
	return 0;
}

/* Funcion que genera las pi para un arreglo de intervalos determinados
* y una funcion especifica.
* REQUIRES:
* 	p		arreglo donde se van almacenar las pi
* 	I		arreglo de intervalos
* 	n		cantidad de intervalos
* 	fun		funcion de densidad
*/
void act4_gen_pi(double *p, double *I, int n, double (*fun)(double))
{
	double	delta = 0;
	int 	i = 0;
	
	assert(I != NULL);
	assert(p != NULL);
	assert(fun != NULL);
	
	delta = I[1] - I[0];
	for (i = 0; i < n; i++) {
		p[i] = act4_monte_carlo(I[i], I[i] + delta, fun) * delta;
	}
	
	
}


