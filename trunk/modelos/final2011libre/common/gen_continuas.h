#ifndef GEN_CONTINUAS_H
#define GEN_CONTINUAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "rdg.h"


typedef double(*funcp_1par_t)(double);
typedef double(*funcp_0par_t)(void);

/* Genera una variable aleatoria continua uniforme en el intervalo (a,b)
 * Funcion densidad de prob: f(x) = 1/(b-a) si a <= x <= b; 0 caso contrario.
 * Funcion distribucion de prob: F(x) = 0 (si x < a); (x-a)/(b-a) a <= x <= b;  1 si x >= b;
*/ 
double rg_gen_uni_cont(double a, double b);

/* generadora de una v.a. exponencial con parametro lambda */
double rg_gen_exp(double lambda);

/* Generacion de una gama con aceptacion y rechazo con parametros (n, lambda) 
 * por medio de la suma de X1 + X2 + ... + Xn exponenciales con parametro Lambda
 */
double gen_gamma(int n, double lambda);

/* Funcion que genera una variable aleatoria de Poisson con parametro
* lambda */
int rg_gen_poisson(double lambda);


/* Funcion que genera una Normal Estandar (N(0,1)) = Z, si queremos |Z| en vez 
 * evitamos la verificacion del U<1/2.
 */
double gen_normal_estandard(void);
double gen_normal_estandard_polar(void);

/* genera una variable aleagoria Normal(mu, sigma), mu = media, sigma = deviacion
 * estandard. */
double gen_normal(double mu, double sigma);


/*! Metodo de aceptacion o rechazo 
* Tenemos 	f(x) funcion de desindad.
* 		g(x) funcion de densidad para generar Y 
*		NOTE: deben coincidir los dominios y g(x) debe generar una variable
			que pertenezca al dominio de f(x).
		c    constante tal que f(Y)/g(Y) <= c >= 1.
		genY funcion que devuelve una v.a. con densidad g(x).(Y = genY())
		NOTE: MODIFICAR genY para cada caso especial, puede ser que no 
		tome parametro alguno genY.
* luego se genera U y se verifica U < f(Y)/(g(Y) * c), con Y = genY();
* */
double metodo_aceptacion_rechazo(funcp_1par_t f, funcp_1par_t g, double c,
				 funcp_0par_t genY);
			  




#endif
