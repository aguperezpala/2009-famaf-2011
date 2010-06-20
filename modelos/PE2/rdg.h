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


/* generadora de una v.a. exponencial con parametro lambda */
double gen_exp(double lambda);


/* Genera un valor según la distribución normal de parámetros (mu,sigma)
 *
 * NOTE: en llamadas sucesivas esta funcion debe ser invocada
 *	 con los mismo valores para mu y sigma.
 *	 Si se decide cambiar estos parametros, el primer valor
 *	 generado tras el cambio debe ser descartado
 *
 * mu = media poblacional
 * sigma = desviacion estandard poblacional
 *
 * PRE: sigma >= 0
 */
double gen_normal (double mu, double sigma);


#endif
