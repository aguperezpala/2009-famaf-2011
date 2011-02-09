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
#include <float.h>


#include "rdg.h"
#include "gen_continuas.h"
#include "gen_discretas.h"
#include "algoritmos_extras.h"



#define PAD 40	/* Definido para arquitectura de 64 bits */



/* Web para obtener los Z alfa/2, http://davidmlane.com/hyperstat/z_table.html */


/*! modo de uso para usar las recursivas 
* 
	double Xpromj1 = 0, Xpromj = 0;
	double Sj1 = 0, Sj= 0;

	Xpromj = sample[0]; -- aca va X(1)
	-- NOTE: fijarse que el bucle va de 1 en adelante, si no deberiamos
	-- usar Xpromj1 = estimar_media_muestral(i+1, Xpromj, sample[i+1]);
	-- Sj1 = estimar_varianza_muestral(i+1, Sj, Xpromj1, Xpromj);
	-- si i empezase de 0
	for(i = 1; i < SIZEMUESTRA; i++){
		Xpromj1 = estimar_media_muestral(i, Xpromj, sample[i]);
		Sj1 = estimar_varianza_muestral(i, Sj, Xpromj1, Xpromj);
		
		Xpromj = Xpromj1;
		Sj = Sj1;
		
       }
*/

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


/*! ********************** METODO DE BOOSTRAP: ****************************************
 * Recordemos que X|(n) ~ N(X|(n), sigma/sqrt(n)). Ademas sabemos que ECM[estimador, parametro] ~
 * ECMempirico[estimador, parametro empirico], en particular E[X|(n), mu empirico] = Varianza(X|(n)) =
 * simga/sqrt(n), Osea se calcula por medio del mu empirico.
 * Revisar problema boostrap_prob practico 6
*/


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
 * TODO: verificar esto porque puede estar andando mal
 */
double bootstrap_varianza (double *sample, unsigned int N);




/*! Calcula el valor de una Ji-Cuadrado */

/* Returns the value of the beta function B(z, w). */
float beta(float z, float w);


/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x);


/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x);

/* Para una funcion de distribucion chi-cuadrada con ciertos gradosLibertad,
 * calcula la probabilidad acumulada: P (chi-cuadrada > value)
 * Recordando que T = sumatoria pow((Ni - n*pi,2)/n*pi from i=1 to n, donde
 * los Ni = cantidad de Xi (Xi son los valores observados, osea la muestra, hasta n)
 * que caen en el intervalo i (que es [yi-1, yi]). Los pi = ProbHipotesisNula(Yi = i)
 * osea la probabilidad asignada a cada intervalo i de la Hipotesis nula.
 * n es el tamanio de la muestra.
 * Luego t = T evaluado, y cuando n es grande T ~ Ji-cuadrado(k-1 - m grados de libertad)
 * Con k = rango de la v.a H0 (que es igua a la cantidad de intervalos) y m cantidad de parametros 
 * desconocidos (por ejemplo si no se conoce bien la funcion de distribucion de H0 
 * -se supone que es una exponencial de parametro lambda, sin conocerlo- entonces
 * m = 1 = lambda).
 */
double chi_cuadrada (int gradosLibertad, double value);



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~  SIMULACION  -  ESTADISTICOS  ~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


/* Estadístico del test Ji-cuadrado para una muestra 'sample' de 'n' valores
 * Los intervalos de agrupacion de resultados deben estar en el parametro 'I'
 * p[i] == "probabilidad de caer en el intervalo Int(i)"
 *
 * Se define al i-esimo intervalo Int(i) como:
 *	Int(i) = [ I[i] , I[i+1] )
 * y para el ultimo intervalo vale que:
 *	Int(k) = [ I[k] , inifinity )
 * Recordar que k = Rango de la v.a H0 (equivalente a la cantidad de intervalos)
 *
 * PRE: sample != NULL	&&  n == #(sample)
 *	I != NULL	&&  k == #(I)
 *	p != NULL	&&  k == #(p)
 * NOTE:Para simular generamos varios t-valores, mediante la funcion de prob
 * de H0 (los pi), y determinamos de esta forma la proporcion de los t-valores
 * generados en cada simulacion de aquellos que son mayores que el t (calculado
 * con los datos observados) y sacamos la proporcion de los t' simulados aquellos
 * que son mayores que t / cantidad de t' generados. Luego esto es el p-valor
 * Osea: p-valor = #{t' : t' > t} / #{t' simulados}
 */
double ji_cuadrado (double *sample, unsigned int n,
		    double *I, unsigned int k, double *p);


/* Estadístico (D) del test Kolmogorov-Smirnov de una muestra 'sample' con 'n' datos
 * 'F' es la función de probabilidad teórica a aplicar sobre los datos
 * Donde D = d = max x |F(x) - Fempirica(e)|
 * Luego p-valor = #{i : Di > d}/#{Di simulados} (se calcula por simulacion)
 * 
 *NOTE: Revisar problemas 3/4 del p7 carlox, se usan siempre Uniformes (las muestras)
 * ya que no depende de la distribucion!. 
 * PRE: sample != NULL  &&  n == #(sample)
 *	F != NULL
 */
 double kolmogorov_smirnov (double *sample, unsigned int n, double (*F) (double));


 
 

/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~ AUXILIARES (IGNORAR) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


/* Returns the value n! as a floating-point number. */
float factrl(int n);

float bico(int n, int k);

float factln(int n);

/* Returns the value ln[Γ(xx)] for xx > 0. */
float gammln(float xx);

/* Returns the incomplete gamma function P (a, x) evaluated by its series 
* representation as gamser.
* Also returns ln Γ(a) as gln.
*/
void gser(float *gamser, float a, float x, float *gln);

/* Returns the incomplete gamma function Q(a, x) evaluated by its continued 
* fraction representation as gammcf. Also returns ln Γ(a) as gln.
*/
void gcf(float *gammcf, float a, float x, float *gln);




#endif
