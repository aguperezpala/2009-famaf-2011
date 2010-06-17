#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include <limits.h>
#include "rdg.h"



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~ GENERADORES DE VALORES ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


/** ### MZRAN13 */

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


/** ### RAN2 */

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


/** ### PROBABILIDAD ARBITRARIA */
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
	/* 1 == sum(p[i]) => No deberíamos habernos pasado de n */
	assert (i<n);
	
	return X[i];
}


/** ### GEOM */
/* Genera un valor según la distribución geométrica de parámetro 'p' */
unsigned int gen_geom (double p)
{
	double U = 0.0, q = 1.0-p;
	struct timeval tv;
	
	if (!INIT) {
		/* Inicializamos ran2 */
		gettimeofday(&tv, NULL);
		IDUM = (long) -((tv.tv_sec << PAD) >> PAD);
		if (IDUM > 0)
			IDUM = -IDUM;
		INIT = true;
	}
/*	U = mzran13()/(double)ULONG_MAX;
*/	U = ran2 (&IDUM);
	return floor (log (U) / log (q)) + 1;
}


/** ### BIN */
/* Genera un valor según la distribución Binomial de parámetros (n,p) */
unsigned int gen_bin (unsigned int n, double p)
{
	double	i = 0.0,
		U = mzran13() / (double) ULONG_MAX,
		c = p / (1.0-p),
		pr = pow (1.0-p, (double) n),
		F = pr;
	
	while (U > F) {
		pr = pr * (((double) n - i) / (i+1.0)) * c;
		F += pr;
		i += 1.0;
	}
	
	return (unsigned int) i;
}


/** ### POISSON */
/* Genera un valor según la distribución Posisson de parámetro lambda */
unsigned int gen_posisson (double lambda)
{
	double  i = 0.0,
		U = 0.0,
		p = pow (M_E, -lambda),
		F = p;
	
	U = mzran13() / (double) ULONG_MAX;
	
	while (U > F) {
		p *= (lambda / (i+1.0));
		F += p;
		i += 1.0;
	}
	
	return (unsigned int) i;
}


/** ### EXP */
/* Genera un valor según la distribución exponencial de parámetro lambda */
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
	return (((double) -1.0/ (double) lambda) * log(U));
}


/** ### NORMAL */
/* Genera dos valores según la distribución normal de parámetros (mu,sigma)
 * Los valores son devueltos en los argumentos N1 y N2
 *
 * mu = media poblacional
 * sigma = desviacion estandard poblacional
 *
 * PRE: N1 != NULL
 *	N2 != NULL
 */
void gen_normal (double mu, double sigma, double *N1, double *N2)
{
	double	U = 0.0,
		V1 = 0.0, V2 = 0.0,	/* Coordenadas del punto en el circulo */
		X = 0.0, Y = 0.0,	/* V.A. ~ N(0,1) a generar */
		S = 0.0;		/* V1^2 + V2^2 */
	
	assert (N1 != NULL);
	assert (N2 != NULL);
	
	/* Generamos el punto dentro del circulo de radio 1 */
	do {
		U = mzran13() / (double) ULONG_MAX;
		V1 = (U * 2.0) - 1.0;
		U = mzran13() / (double) ULONG_MAX;
		V2 = (U * 2.0) - 1.0;
		/* V1,V2 ~ U(-1,1) */
		S = V1*V1 + V2*V2;
	} while (S > 1.0);
	
	X = sqrt (-2.0 * log(S) / S) * V1;
	Y = sqrt (-2.0 * log(S) / S) * V2;
	
	*N1 = X * sigma + mu;
	*N2 = Y * sigma + mu;
	
	return;
}


/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~ CALCULADORES DE PROBABILIDAD ~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */



/** ### BIN */
/* Rellena el arreglo 'prob' con los valores P(X=x) para los x en {0..n}
 * según la función de distribución de masa de una Binomial (n,p)
 *
 * PRE: prob != NULL  &&  n+1 == #(prob)
 */
void prob_bin (unsigned int n, double p, double *prob)
{
	unsigned int i = 0;
	double	c = p / (1.0-p),
		pi = pow (1.0-p, (double) n);
	
	assert (prob != NULL);
	
	do {
		prob[i] = pi;
		pi = pi * (((double)(n-i)) / (double)(i+1)) * c;
		i++;
	} while (i <= n);
	
	return;
}

/* Imprime por pantalla los valores P(X=x) para los x en {0..n}
 * según la función de distribución de masa de una Binomial (n,p) */
void print_bin (unsigned int n, double p)
{
	double	i = 0.0,
		c = p / (1.0-p),
		pi = pow (1.0-p, (double) n);
	
	printf ("Bin (n=%u, p=%.4f)\n", n, p);
	printf ("p%.0f = %.6f\n", i, pi);
	
	for (i=0.0 ; (unsigned int) i < n ; i+=1.0) {
		pi = pi * (((double) n - i) / (i+1.0)) * c;
		printf ("p%.0f = %.6f\n", i+1.0, pi);
	}
	
	return;
}


/** ### BETA */
/* Returns the value of the beta function B(z, w). */
float beta(float z, float w)
{
	float gammln(float xx);
	return exp(gammln(z)+gammln(w)-gammln(z+w));
}


/** ### GAMMA */
/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x)
{
	float gamser,gammcf,gln;
	if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammp");
	
	if (x < (a+1.0)) {
		/* Use the series representation. */
		gser(&gamser,a,x,&gln);
		return gamser;
	} else {
		/* Use the continued fraction representation... */
		gcf(&gammcf,a,x,&gln);
		/* ... and take its complement. */
		return 1.0-gammcf;
	}
}



/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x)
{
	float gamser,gammcf,gln;
	
	if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammq");
	
	if (x < (a+1.0)) {
		/* Use the series representation... */
		gser(&gamser,a,x,&gln);
		/* ... and take its complement. */
		return 1.0-gamser;
	} else {
		/* Use the continued fraction representation. */
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}


/** ### Ji-2 */
double chi_cuadrada(int gradosLibertad, double value)
{
	return gammq((double)gradosLibertad/(double)2.0,value/(double)2.0);
}



/** ------------------------------------------------------------------------- */
/** ~~~~~~~~~~~~~~~~~~~~~~~ AUXILIARES (IGNORAR) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */


/*! para gcf */
/* Maximum allowed number of iterations. */
#define ITMAX 100
/* Relative accuracy. */
#define EPSI 3.0e-7
/* Number near the smallest representable */
#define FPMIN 1.0e-30


/* Returns the value n! as a floating-point number. */
float factrl(int n)
{
	float gammln(float xx);
	static int ntop=4;
	/*	Fill in table only as required.*/
	static float a[33]={1.0,1.0,2.0,6.0,24.0};
	int j;
	if (n < 0) printf("Negative factorial in routine factrl");
	if (n > 32) return exp(gammln(n+1.0));
	/*	Larger value than size of table is required. Actually, this big a value is going to overflow
	on many computers, but no harm in trying.
	Fill in table up to desired value.*/
	while (ntop<n) {
		j=ntop++;
		a[ntop]=a[j]*ntop;
	}
	return a[n];
}



float bico(int n, int k)
{
	float factln(int n);
	return floor(0.5+exp(factln(n)-factln(k)-factln(n-k)));
	/* The floor function cleans up roundoff error
	* for smaller values of n and k. */
}



float factln(int n)
{
	float gammln(float xx);
	/* A static array is automatically initialized to zero.*/
	static float a[101];
	
	if (n < 0) printf("Negative factorial in routine factln");
	if (n <= 1) return 0.0;
	
	if (n <= 100)
		/* In range of table */
		return a[n] ? a[n] : (a[n]=gammln(n+1.0));
	else
		/* Out of range */
		return gammln(n+1.0);
}



/* Returns the value ln[Γ(xx)] for xx > 0. */
float gammln(float xx)
{
	/* Internal arithmetic will be done in double precision, a nicety that 
	* you can omit if five-figure accuracy is good enough. */
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
  0.1208650973866179e-2,-0.5395239384953e-5};
  int j;
  y=x=xx;
  tmp=x+5.5;
  tmp -= (x+0.5)*log(tmp);
  ser=1.000000000190015;
  for (j=0;j<=5;j++) ser += cof[j]/++y;
  return -tmp+log(2.5066282746310005*ser/x);
}



/* Returns the incomplete gamma function P (a, x) evaluated by its series 
 * representation as gamser.
 * Also returns ln Γ(a) as gln.
 */
void gser(float *gamser, float a, float x, float *gln)
{
	float gammln(float xx);
	int n;
	float sum,del,ap;
	*gln=gammln(a);
	if (x <= 0.0) {
		if (x < 0.0) printf("x less than 0 in routine gser");
		*gamser=0.0;
		return;
	} else {
		ap=a;
		del=sum=1.0/a;
		for (n=1;n<=ITMAX;n++) {
			++ap;
			del *= x/ap;
			sum += del;
			if (fabs(del) < fabs(sum)*EPSI) {
				*gamser=sum*exp(-x+a*log(x)-(*gln));
				return;
			}
		}
		printf("a too large, ITMAX too small in routine gser");
		return;
	}
}


/* Returns the incomplete gamma function Q(a, x) evaluated by its continued 
 * fraction representation as gammcf. Also returns ln Γ(a) as gln.
 */
void gcf(float *gammcf, float a, float x, float *gln)
{
	float gammln(float xx);
	int i;
	float an,b,c,d,del,h;
	*gln=gammln(a);
	/* Set up for evaluating continued fraction */
	b=x+1.0-a;
	/* by modified Lentz’s method (§5.2) */
	c=1.0/FPMIN;
	/* with b0 = 0. */
	d=1.0/b;
	h=d;
	/* Iterate to convergence. */
	for (i=1;i<=ITMAX;i++) {
		an = -i*(i-a);
		b += 2.0;
		d=an*d+b;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=b+an/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < EPSI) break;
	}
	if (i > ITMAX) printf("a too large, ITMAX too small in gcf");
	/* Put factors in front. */
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
}
