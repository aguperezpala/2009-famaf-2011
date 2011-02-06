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
#include "algoritmos_extras.h"


/* Web para obtener los Z alfa/2, http://davidmlane.com/hyperstat/z_table.html */



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


/* Funcion que calcula la media muestral sin hacer recursion, ineficiente
* pero segura
* Requiere la muestra de tamanio N
*/
double estimar_media_muest_inef(double *sample, int N);

/* Funcion que calcula la varianza muestral sin hacer recursion, ineficiente
 * pero segura
 * Requiere la media muestral y la muestra de tamanio N
 */
double estimar_var_muest_inef(double *sample, int N, double mediaMuestral);


/*! Método de Bootstrap para estimar el Error Cuadrático Medio (ECM)
* del estimador "Media muestral" con respecto a la verdadera media µ,
* dada una muestra de 'n' datos pasada como primer argumento
*
* Utiliza Montecarlo para devolver un valor que aproxima al ECM exacto
*
* PRE: sample != NULL
*/
double bootstrap_media (double *sample, unsigned int N);


/*! Metodo de Bootstrap para calcular el ECM de la Varianza muestral.
 * Toma una muestra sample de n elementos.
 * En caso de que N muy grande (definido en el .c) usa montecarlo para
 * aproximar 
 */
double bootstrap_varianza (double *sample, unsigned int N);


#endif
