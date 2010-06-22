#include "actividad4.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>


/* Funcion que genera el valor de la integral en un intervalo entre a y b
 * y la funcion fun.
 */
double act4_monte_carlo(double a, double b, double (*fun)(double))
{
	double U = 0;
	double acum = 0;
	int i = 0;
	
	for(i = 0; i < MONTECARLO_BOUND; i++) {
		U = mzran13()/(double)ULONG_MAX;
		U = (U * (b-a)) + a;
		
		acum += fun(U);
	}
	
	return (acum/(double)MONTECARLO_BOUND)*(b-a);
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
		p[i] = act4_monte_carlo(I[i], I[i] + delta, fun);
	}
	
	
}


/* Genera valores en Xvals que están en el medio de los intervalos
 * representados en el arreglo I.
 * De tal forma: I[j] < Xvals[j] < I[j+1]
 *
 * PRE:	I != NULL
 *	Xvals != NULL
 *	NI == size(I) == size(Xvals)
 */
void act4_gen_values (double *I, unsigned int NI, double *Xvals)
{
	unsigned int i = 0;
	
	assert (I != NULL);
	assert (Xvals != NULL);
	
	for (i=0 ; i<NI-1 ; i++) {
		Xvals[i] = (I[i] + I[i+1]) / 2.0;
	}
	Xvals[NI-1] = I[NI-1] + (I[NI-1] - I[NI-2]) / 2.0;
	
	return;
}
