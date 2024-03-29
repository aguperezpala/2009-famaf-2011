#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <limits.h>
#include "rdg.h"

/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~ ### MZRAN13 ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

/* Default seed values */
unlong x=521288629, y=362436069, z=16163801, c=1, n=1131199209;

/* Main algorithm */
unlong mzran13 (void) {
	long int s;
	
	if (y>x+c) {
		s=y-(x+c);
		c=0;
	} else {
		s=y-(x+c)-18;
		c=1;
	}
	x=y;
	y=z;
	return (z=s) + (n=69069*n+1013904243);
}

/* Initialization routine (seed modification) */
void ran13set (unlong xx, unlong yy, unlong zz, long nn) {
	x=xx;
	y=yy;
	z=zz;
	n=nn;
	c=y>z;
}



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ### RAN2 ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

#define PAD 40	/* Definido para arquitectura de 64 bits */
long IDUM = -1;
bool INIT = false;

#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774

/* Copyright (C) 1988-1992 by Cambridge University Press. Programs Copyright (C)
* 1988-1992 by Numerical Recipes Software. Permission is granted for users of
* the World Wide Web to make one paper copy for their own personal use. Further
* reproduction, or any copying of machine-readable files (including this one) 
* to anyserver computer, is strictly prohibited. To order Numerical
* Recipesbooks and diskettes, go to http://world.std.com/~nr or call
* 1-800-872-7423 (North America only), or send email to trade@cup.cam.ac.uk
* (outside North America).
*/

#define IR1 12211
#define IR2 3791
#define NTAB 32			/* Shuffling table length */
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7		/* Return values' precision */
#define RNMX (1.0-EPS)		/* Higest possible return value */


/* Long period (> 2 × 10^18) random number generator of L’Ecuyer with Bays-Durham
* shuffle and added safeguards. Returns a uniform random deviate between 0.0
* and 1.0 (exclusive of the endpoint values).
* CALL WITH IDUM A NEGATIVE INTEGER TO INITIALIZE; thereafter, do not alter
* idum between successive deviates in a sequence. RNMX should approximate the
* largest floating value that is less than 1.
*/
float ran2(long *idum)
{
	int j;
	long k;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];
	float temp;
	
	/*Initialize.*/
	if (*idum <= 0) {
		/*Be sure to prevent idum = 0.*/
		if (-(*idum) < 1)
			*idum=1;
		else
			*idum = -(*idum);
		idum2=(*idum);
		/* Load the shuffle table (after 8 warm-ups).*/
		for (j=NTAB+7;j>=0;j--) {
			k=(*idum)/IQ1;
			*idum=IA1*(*idum-k*IQ1)-k*IR1;
			if (*idum < 0)
				*idum += IM1;
			if (j < NTAB)
				iv[j] = *idum;
		}
		iy=iv[0];
	}
	
	/* Start here when not initializing.*/
	k=(*idum)/IQ1;
	/* Compute idum=(IA1*idum) % IM1 without*/
	*idum=IA1*(*idum-k*IQ1)-k*IR1;
	/* overflows by Schrage’s method.*/
	if (*idum < 0)
		*idum += IM1;
	k=idum2/IQ2;
	/* Compute idum2=(IA2*idum) % IM2 likewise.*/
	idum2=IA2*(idum2-k*IQ2)-k*IR2;
	if (idum2 < 0)
		idum2 += IM2;
	/* Will be in the range 0..NTAB-1.*/
	j=iy/NDIV;
	/* Here idum is shuffled, idum and idum2 are*/
	iy=iv[j]-idum2;
	/* combined to generate output.*/
	iv[j] = *idum;
	if (iy < 1)
		iy += IMM1;
	
	if ((temp=AM*iy) > RNMX)
		return RNMX; /* Because users don’t expect endpoint values.*/
		else
			return temp;
}





/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~ ### PROBABILIDAD ARBITRARIA ### ~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

/* Genera un valor de entre los listados en 'X',
 * segun la distribucion arbitraria especificada en 'p'
 *
 * Emplea el método de la transformada inversa
 *
 * PRE: X != NULL
 *	p != NULL
 *	n == #(X) == #(p)
 */
double gen_prob (double *X, double *p, unsigned int n)
{
	double F = 0.0, U = 0.0;
	unsigned int i = 0;
	
	assert (X != NULL);
	assert (p != NULL);
	
	U = mzran13()/(double)ULONG_MAX;
	F = p[i];
	
	while (U > F && i<n) {
		i++;
		F += p[i];
	}
/*	1 == sum(p[i]) => No deberíamos habernos pasado de n 
	assert (i<n);
*/
	/* Es aserción puede ser rota a raíz de que se usan aproximaciones 
	 * para generar los arreglos p[] */
	if (i == n) i = n-1;
	
	return X[i];
}


/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ### EXP ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

/* generadora de una v.a. exponencial con parametro lambda */
double gen_exp (double lambda)
{
	double U = 0;
	struct timeval tv;
	
	if (!INIT) {
		/* Inicializamos ran2 */
		gettimeofday(&tv, NULL);
		IDUM = (long) -((tv.tv_sec << PAD) >> PAD);
		if (IDUM > 0)
			IDUM = -IDUM;
		INIT = true;
	}
	
	/* NOTE:  mzran13 genera    determinismo
	 *	    ran2  genera no determinismo
	 */
	U = ran2 (&IDUM);
/*	U = mzran13()/(double)ULONG_MAX;
*/		
	return (-log(U)/lambda);
}



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ ### NORMAL ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

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
double gen_normal (double mu, double sigma)
{
	double	U = 0.0,
		V1 = 0.0, V2 = 0.0,	/* Coordenadas del punto en el circulo */
		S = 0.0;		/* V1^2 + V2^2 */
	/* V.A. ~ N(0,1) a generar */
	static double	X = 0.0,
			Y = 0.0;
	static bool lazy = false;
	
	assert (sigma >= 0.0);
	
	if (lazy) {
		/* Devolvemos el segundo resultado generado la llamada ant. */
		lazy = false;
		return Y;
	
	} else {
		/* Esta vez hay que laburar, generamos dos valores */
		do {
			/* Generamos el punto dentro del circulo de radio 1 */
			U = mzran13() / (double) ULONG_MAX;
			V1 = (U * 2.0) - 1.0;
			U = mzran13() / (double) ULONG_MAX;
			V2 = (U * 2.0) - 1.0;
			
			S = V1*V1 + V2*V2;
			
		} while (S > 1.0);
		
		/* Primer valor generado */
		X = sqrt (-2.0 * log(S) / S) * V1;
		X = X * sigma + mu;
		
		/* Segundo valor generado */
		Y = sqrt (-2.0 * log(S) / S) * V2;
		Y = Y * sigma + mu;
		
		lazy = true;
		return X;
	}
}
