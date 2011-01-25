#ifndef GEN_DISCRETAS_H
#define GEN_DISCRETAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "rdg.h"
#include "gen_continuas.h"

/*!  METODO TRANSFORMADA INVERSA
 * Queremos generar X ~ con func de masa de probabilidad P(X = xj) = pj. Sum pj = 1
 * Para esto generamos U(0,1) y X = { x0 si U < p0, x1 si p0 <= U < p1, en general
 * xj -> p0+..+pj-1 <= U < p0+..+pj}
 * Esto porque P(a <= U < b) = b - a para 0 < a < b < 1, p0+..+pj - (p0+..+pj-1) = pj
 */
/* Toma un arreglo con las pi, el size de las pi y estas pi deben estar ordenadas
 * en relacion a las xi. Devuelve el indice [0,size-1], por lo que el valor de
 * la v.a generada es X[result] con result = indice devuelto.
 * NOTE: es mas eficiente si se ordenan los pi en forma decreciente.
 */
int disc_transf_inversa(double *pi);


/* Generacion de una v.a uniforme discreta entre [a,b] */
int disc_gen_uniforme(int a, int b);



#endif
