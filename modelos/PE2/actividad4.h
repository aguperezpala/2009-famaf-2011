#ifndef ACTIVIDAD4_H
#define ACTIVIDAD4_H

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>



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
int act4_gen_intervals(int begin, int end, int n, double *I);




#endif
