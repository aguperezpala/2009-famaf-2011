#ifndef ACTIVIDAD4_H
#define ACTIVIDAD4_H

#define _GNU_SOURCE
#include <limits.h>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "rdg.h"

#define MONTECARLO_BOUND	1000


/* Funcion que genera el valor de la integral en un intervalo entre a y b
* y la funcion de densidad fun por medio del metodo de montecarlo.
*/
double act4_monte_carlo(double a, double b, double (*fun)(double));

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
int act4_gen_intervals(double begin, double end, int n, double *I);

/* Funcion que genera las pi para un arreglo de intervalos determinados
 * y una funcion especifica.
 * REQUIRES:
 * 	p		arreglo donde se van almacenar las pi
 * 	I		arreglo de intervalos
 * 	n		cantidad de intervalos
 * 	fun		funcion de densidad
 */
void act4_gen_pi(double *p, double *I, int n, double (*fun)(double));


/* Genera valores en Xvals que están en el medio de los intervalos
 * representados en el arreglo I.
 * De tal forma: I[j] < Xvals[j] < I[j+1]
 *
 * PRE:	I != NULL
 *	Xvals != NULL
 *	NI == size(I) == size(Xvals)
 */
void act4_gen_values (double *I, unsigned int NI, double *Xvals);



#endif
