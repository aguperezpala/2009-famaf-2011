#include <math.h>
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
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ### EXP ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */

/* generadora de una v.a. exponencial con parametro lambda */
double gen_exp(double lambda)
{
	double U = 0;
	
	U = mzran13()/(double)ULONG_MAX;
	
	return (((double) -1.0/ (double) lambda) * log(U));
}
