#include "actividad4.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>

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
int act4_gen_intervals(int begin, int end, int n, double *I)
{
	double delta = 0;
	int i = 0;
	
	assert(I != NULL);
	delta = ((double)(end-begin))/(double)n;
	
	if(delta <= 0)
		return -1;
	
	I[0] = begin;
	for(i = 1; i < n; i++)
		I[i] = (double) begin + ((double)i) * delta;
	
	return 0;
}

