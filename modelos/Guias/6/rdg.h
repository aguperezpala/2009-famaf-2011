#ifndef RANDOM_DEVIATES_GENERATORS_H
#define RANDOM_DEVIATES_GENERATORS_H


typedef unsigned long int unlong;


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


/* generadora de una v.a. exponencial con parametro lambda */
double gen_exp(double lambda);


/* Returns the value of the beta function B(z, w). */
float beta(float z, float w);


/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x);


/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x);


/* generadora de chi-cuadrada */
double chi_cuadrada(int gradosLibertad, double value);






/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~ ### AUXILIARES (IGNORAR) ### ~~~~~~~~~~~~~~~~~~~~~~~~ */
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
