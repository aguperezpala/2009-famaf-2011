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

typedef double(*FUNC_TI_PTR)(int);

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
/* Otra opcion es darle la funcion de distribucion para obtener los pi directamente
 * Aca perdemos eficiencia pero es mas rapido de programar 
 * NOTE: deberiamos cambiar el puntero de funcion si la funcion toma mas
 * argumentos... 
 * El 2` argumento es para indicar desde donde debe comenzar la F[j] argumento 
 */
int disc_trasnf_inversa_func(FUNC_TI_PTR funcDistr);
int disc_trasnf_inversa_func2(FUNC_TI_PTR funcDistr, int j);


/* Generacion de una v.a uniforme discreta entre [a,b). 
 * NOTE: Es cerrado por abajo y abierto por arriba!!*/
int disc_gen_uniforme(int a, int b);

/* Generacion de una Geometrica: Cuenta el numero de fracasos hasta el primer
 * exito con probabilidad p. P(X = j) = pow(j-1, 1-p) * p.
 */
int disc_gen_geometrica(double p);


/* Generacion X~Bn(r,p) Binomial Negativa: cuenta la cantidad de ensayos hasta
 * conseguir r exitos. X = X1 + X2 + ... + Xn, con Xi ~ Geometricas.
 * P(X = i) = Conv(i-1, r-1) * pow(1-p, i-r) * pow(p,r)
 */
int disc_gen_bin_negativa(int r, double p);

/* Generacion Poisson: P(X=x) = exp(-lambda) * pow(lambda, x) / fact(x); */
int disc_gen_poisson(double lambda);

/* Generacion de una Binomial: n ensayos fijos, cuenta la cantidad de exitos con
 * probabilidad p de exito cada uno independientes los ensayos.
 * P(X=j)=Conv(n,j) * pow(p, j) * pow(1-p, n-j).
 */
int disc_gen_binomial(int n, double p);


/*!*****************************************************************************/




/*! Metodo de acceptacion y rechazo:
 * Se quiere calcular X, con {pj}, j = 1,2,....k Se tiene un metodo eficiente para
 * generar Y v,a con prob {qj} j=1,2...k Debe cumplir que pj != 0 ==> qj != 0
 * para todo j. Ademas: c > 1 : pj/qj <= c para todo j. La Y debe adoptar valores
 * dentro del rango de X.
 * Toma como argumentos la funcion que genera la func de masa de X : P[X = i],
 * la funcion que genera los qj = P[Y=j], que en general es una uniforme por lo que
 * uno de los parametros esta de mas (solo usamos el tamanio y no el i ya que 
 * para todo i, qj = 1/n). Pero mejos mas generico.
 */
int disc_aceptacion_rechazo(double (*funcDistr_Pj)(int), double (*funcDistrRechazo_Qj)(int, int), 
			    double cota, int k);




/*!*****************************************************************************/

/*
* Metodo de composicion: sea X = alfa * X1 + (1-alfa) * X2, con P1 func de dens
* de prob de X1, P2 de X2 (deben cumplir que la sumatoria de las P1j = 1. idem P2j).
* Usamos la transformada inversa para calcular las P1j, P2j
* El j es para determinar desde donde comienzan los pj (j = 1...., o j = 0..., etc)
*/ 
int disc_metodo_composicion(double (*P1j)(int), double (*P2j)(int), double alfa, int j);

#endif
