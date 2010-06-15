#include "distrib.h"

/* 	Returns the value n! as a floating-point number. */
float factrl(int n)
{
	float gammln(float xx);
	static int ntop=4;
//	Fill in table only as required.
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
//	The floor function cleans up roundoff error for smaller values of n and k.
}



float factln(int n)
{
	float gammln(float xx);
	//	A static array is automatically initialized to zero.
	static float a[101];
	if (n < 0) printf("Negative factorial in routine factln");
	if (n <= 1) return 0.0;
	if (n <= 100) return a[n] ? a[n] : (a[n]=gammln(n+1.0)); /*In range of table.
		Out of range of table.*/
		else return gammln(n+1.0);
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
			if (fabs(del) < fabs(sum)*EPS) {
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
	//Set up for evaluating continued fraction
	b=x+1.0-a;
	//by modified Lentz’s method (§5.2)
	c=1.0/FPMIN;
	//with b0 = 0.
	d=1.0/b;
	h=d;
	//Iterate to convergence.
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
		if (fabs(del-1.0) < EPS) break;
	}
	if (i > ITMAX) printf("a too large, ITMAX too small in gcf");
	//Put factors in front.
	*gammcf=exp(-x+a*log(x)-(*gln))*h;
}



/* Returns the incomplete gamma function P (a, x). */
float gammp(float a, float x)
{
	float gamser,gammcf,gln;
	if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammp");
//	Use the series representation.
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
		return gamser;
//		Use the continued fraction representation
	} else {
		gcf(&gammcf,a,x,&gln);
//		and take its complement.
		return 1.0-gammcf;
	}
}


/* Returns the value of the beta function B(z, w). */
float beta(float z, float w)
{
	float gammln(float xx);
	return exp(gammln(z)+gammln(w)-gammln(z+w));
}


/* Returns the incomplete gamma function Q(a, x) ≡ 1 − P (a, x). */
float gammq(float a, float x)
{
	float gamser,gammcf,gln;
	if (x < 0.0 || a <= 0.0) printf("Invalid arguments in routine gammq");
//	Use the series representation
	if (x < (a+1.0)) {
		gser(&gamser,a,x,&gln);
//		and take its complement.
		return 1.0-gamser;
//		Use the continued fraction representation.
	} else {
		gcf(&gammcf,a,x,&gln);
		return gammcf;
	}
}

double chi_cuadrada(int gradosLibertad, double value)
{
	return gammq((double)gradosLibertad/(double)2.0,value/(double)2.0);
}

