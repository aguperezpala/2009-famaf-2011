#ifndef RANDOM_DEVIATES_GENERATORS_H
#define RANDOM_DEVIATES_GENERATORS_H


typedef unsigned long int unlong;



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~ GENERADORES DE VALORES ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

/* Long period (> 2 × 10^18) random number generator of L’Ecuyer with Bays-Durham
 * shuffle and added safeguards. Returns a uniform random deviate between 0.0
 * and 1.0 (exclusive of the endpoint values).
 * CALL WITH IDUM A NEGATIVE INTEGER TO INITIALIZE; thereafter, do not alter
 * idum between successive deviates in a sequence. RNMX should approximate the
 * largest floating value that is less than 1.
 */
float ran2 (long *idum);


/* Another random number generator, proposed by Marsaglia and Zaman in 1994
 * It's period is around 2^125 ~ 10^36... MUCH greater than ran2's 10^18
 * Initialization is done using auxiliary routine ran13set. If it's not used
 * default seed values are used.
 */
unlong mzran13 (void);

/* Seeding routine for mzran13 */
void ran13set(unlong xx, unlong yy, unlong zz, long nn);


/* Genera un valor de entre los listados en 'X',
 * segun la distribucion arbitraria especificada en 'p'
 *
 * Emplea el método de la transformada inversa
 *
 * PRE: X != NULL
 *	p != NULL
 *	n == #(X) == #(p)
 */
double gen_prob (double *X, double *p, unsigned int n);


/* Genera un valor según la distribución exponencial de parámetro 'p' */
unsigned int gen_geom (double p);


/* Genera un valor según la distribución Binomial de parámetros (n,p) */
unsigned int gen_bin (unsigned int n, double p);


/* Genera un valor según la distribución Poisson de parámetro lambda */
unsigned int gen_posisson (double lambda);


/* Genera un valor según la distribución geométrica de parametro lambda */
double gen_exp (double lambda);



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~ CALCULADORES DE PROBABILIDAD ~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


/* Rellena el arreglo 'prob' con los valores P(X=x) para los x en {0..n}
 * según la función de distribución de masa de una Binomial (n,p)
 *
 * PRE: prob != NULL  &&  n+1 == #(prob)
 */
void prob_bin (unsigned int n, double p, double *prob);


/* Imprime por pantalla los valores P(X=x) para los x en {0..n}
 * según la función de distribución de masa de una Binomial (n,p) */
void print_bin (unsigned int n, double p);


/* Returns the value of the beta function B(z, w). */
float beta(float z, float w);


/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x);


/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x);


/* Para una funcion de distribucion chi-cuadrada con ciertos gradosLibertad,
 * calcula la probabilidad acumulada: P (chi-cuadrada > value)
 */
double chi_cuadrada (int gradosLibertad, double value);





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
