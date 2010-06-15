#ifndef DISTRIB_H
#define DISTRIB_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/*! para gcf */
/* Maximum allowed number of iterations. */
#define ITMAX 100
/* Relative accuracy. */
#define EPS 3.0e-7
/* Number near the smallest representable */
#define FPMIN 1.0e-30

/** ### Funciones auxiliares ### */

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




/** ### Funciones de distribuciones ### */

/* Returns the value of the beta function B(z, w). */
float beta(float z, float w);

/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x);

/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x);

/* generadora de chi-cuadrada */
double chi_cuadrada(int gradosLibertad, double value);



#endif
