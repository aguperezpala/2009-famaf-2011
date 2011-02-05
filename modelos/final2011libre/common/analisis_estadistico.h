#ifndef ANALISIS_ESTADISTICO_H
#define ANALISIS_ESTADISTICO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "rdg.h"
#include "gen_continuas.h"
#include "gen_discretas.h"


/* Funcion que estima la media muestral (X(n) = sumatoria xi/n from i=1 to n) con los
 * xi datos observados.
 * Para calcularla utilizando la forma recursiva.
 * X| j+1 = X| j + (Xj+1 - X| j)/(j+1)
 * j:		j (nosotros devolveriamos el j+1)
 * Xpromj	X| j (media muestral calculada en el paso anterior)
 * Xj1		Xj+1	el valor de Xj+1
 * 
 * Devuelve X| j+1
 */
double estimar_media_muestral(int j, double Xpromj, double Xj1);

/* Funcion para calcular la varianza muestral (=desviacion estandard al cuadrado)
 * S2 = sumatoria pow((Xi- MediaMuestral(Xn)),2)/(n-1) from i=1 to n
 * Esta funcion la calcula recursivamente (Devuelve S2 j+1):
 * 
 * j		(nosotros devolvemos el j+1)
 * Sj		(el valor anteriormente calculado de la varianza muestral)
 * Xpromj1	(el valor del la media muestral de j+1)
 * Xpromj	(el valor de la media muestral de j)
 */
double estimar_varianza_muestral(int j, double Sj, double Xpromj1, double Xpromj);



#endif
