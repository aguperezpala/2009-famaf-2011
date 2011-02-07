#include "analisis_estadistico.h"


#define  MAX(x,y)  ( ((x) > (y)) ? (x) : (y) )



/******************************************************************************/
double estimar_media_muestral(int j, double Xpromj, double Xj1)
{
	return (Xpromj + (Xj1 - Xpromj)/(j+1));
}

/******************************************************************************/
double estimar_varianza_muestral(int j, double Sj, double Xpromj1, double Xpromj)
{
	return (((j - 1.0)/(double)j)*Sj + ((double)(j+1.0))*pow(Xpromj1-Xpromj,2));
}

/******************************************************************************/
double estimar_media_muest_inef(double *sample, int N)
{
	int i = 0;
	double result = 0;
	
	for(i; i < N; i++)
		result += sample[i];
	
	return result/(double)N;
}

/******************************************************************************/
double estimar_var_muest_inef(double *sample, int N, double mediaMuestral)
{
	int i = 0;
	double result = 0;
	
	for(i; i < N; i++)
		result += pow(sample[i]-mediaMuestral,2);
	
	return result/(double)N;
}


/******************************************************************************/

/******************************************************************************/

/*!*****************************************************************************/

#define SIZE	2
#define MONTECARLO_BOUND 10000		/* iteraciones de montecarlo */


/** Estructura Media muestral */
double	media[SIZE] = {0.0, 0.0};
unsigned int	ma = 0,  /* media[ma] = media anterior  = X(n-1) */
ms = 0;  /* media[ms] = media siguiente = X(n)   */

/** Estructura Varianza muestral */
double	var[SIZE] = {0.0, 0.0};		/* Varianza muestral */
unsigned int	va = 0,  /* var[va] = varianza anterior  = S^2(n-1) */
vs = 0;  /* var[vs] = varianza siguiente = S^2(n)   */



/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
* devuelve el "proximo" valor de la media muestral = X(n)
*/
static double media_m (double Xn, double n)
{
	/* Recordar:	media[ma] = media anterior  = X(n-1) *
	*		media[ms] = media siguiente = X(n)   *
	*						     */
	ma = ms;
	ms = (ms+1) % SIZE;
	media[ms] = media[ma] + (Xn-media[ma]) / n;
	
	return media[ms];
}


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
* devuelve el "proximo" valor de la varianza muestral = S^2(n)
*
* NOTE: ASUME que (media[ma] , media[ms]) tienen los valores (X(n-1) , X(n))
* 	 respectivamente (ver arriba la funcion media_m, "Media muestral")
*/
static double var_m (double Xn, double n)
{
	/* Recordar:	var[va] = varianza anterior  = S^2(n-1) *
	*		var[vs] = varianza siguiente = S^2(n)	*
	*							*/
	if (n > 1) {
		va = vs;
		vs = (vs+1) % SIZE;
		var[vs] = ((n - 1.0) / n) * var[va] + \
		(n + 1.0) * pow (media[ms] - media[ma], 2.0);
	}
	
	return var[vs];
}


/* Reinicializa la media, como si volviesemos al paso 0 */
static void reset_media_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		media[i] = 0.0;
	return;
}


/* Reinicializa la varianza, como si volviesemos al paso 0 */
static void reset_var_m (void)
{
	int i = 0;
	for (i=0 ; i<SIZE ; i++)
		var[i] = 0.0;
	return;
}


double bootstrap_media (double *sample, unsigned int n)
{
	double ecm = 0.0;
	unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;
	unsigned long N = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Xc = 0.0;	/* Media muestral de una configuración */
	int *config = NULL;	/* Configuración posible dada la muestra */
	
	
	/* Metemos la media empírica de la muestra real en Xe */
	for (i=0 ; i<n ; i++)
		Xe += sample[i] / (double) n;
	
	N = pow ((double) n, (double) n);
	ecm = 0.0;
	
	/* Aca definimos tolerancia para los datos en 6 valores, si son menos de
	 * 6 calculamos todas las posibles configuraciones (pow(5,5))*/
	if (n < 6) {
		/* Vamos a calcular las cosas de manera exacta,
		* considerando todas las configuraciones posibles */
		config = (int *) calloc (n , sizeof(int));
		
		for (i=0 ; i<N ; i++) {
			
			/* Determinamos cual es esta configuración */
			row = i / n;
			col = i % n;
			config[row] += col;
			
			/* Guardamos su media muestral en Xc */
			reset_media_m ();
			for (j=0 ; j<n ; j++)
				Xc  = media_m (sample[config[j]], j+1);
				
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Xc - Xe, 2.0)) / (double) N;
		}
		free (config);
		config = NULL;
		
	} else {
		/* Vamos a aproximar el ECM empleando Montecarlo */
		
		for (i=0 ; i<MONTECARLO_BOUND ; i++) {
			
			/* Guardamos en Xc la media muestral de una
			* configuracion aleatoria de valores de la muestra */
			reset_media_m ();
			for (j=0 ; j<n ; j++) {
				k = disc_gen_uniforme(0, n);
				Xc = media_m (sample[k], j+1);
			}
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Xc - Xe, 2.0)) / (double) MONTECARLO_BOUND;
			/*
			if (!(i%500))
				printf ("loop # %u\tecm == %.4f\n", i, ecm);*/
		}
	}
	
	return ecm;
}

/******************************************************************************/
double bootstrap_varianza (double *sample, unsigned int n)
{
	double ecm = 0.0;
	unsigned int i = 0, j = 0, k = 0, row = 0, col = 0;
	unsigned long N = 0;
	double Xe = 0.0;	/* Media empírica de la muestra */
	double Ve = 0.0;	/* Varianza empirica */
	double Vc = 0.0;	/* Varianza muestral de una configuración */
	int *config = NULL;	/* Configuración posible dada la muestra */
	double aux = 0;
	
	/* Metemos la media empírica de la muestra real en Xe */
	for (i=0 ; i<n ; i++)
		Xe += sample[i] / (double) n;
	
	/* calculamos la varianza empirica = Sumatoria pow((Xi-Xe),2)/n fomr 1 to n */
	for(i = 0; i < n; i++) {
		Ve += pow(sample[i]-Xe,2)/(double)n;
	}
	
	N = pow ((double) n, (double) n);
	ecm = 0.0;
	
	/* Aca definimos tolerancia para los datos en 6 valores, si son menos de
	* 6 calculamos todas las posibles configuraciones (pow(5,5))*/
	if (n < 1) {
		/* Vamos a calcular las cosas de manera exacta,
		* considerando todas las configuraciones posibles */
		config = (int *) calloc (n , sizeof(int));
		
		for (i=0 ; i<N ; i++) {
			
			/* Determinamos cual es esta configuración */
			row = i / n;
			col = i % n;
			config[row] += col;
			
			/* Guardamos su varianza muestral en Vc */
			reset_var_m ();
			reset_media_m();
			for (j=0 ; j<n ; j++){
				aux = media_m (sample[config[j]], j+1);
				Vc  = var_m(sample[config[j]], j+1);
				printf("Seleccionada X[%d]\t", config[j]);
				/*printf("Media: %G\tVarianza: %G\n", aux, Vc);*/
			}
			printf("\n");
			/*printf("Media: %G\n", media[ms]);
			printf("Varianza: %G\n\n\n", Vc);*/
			/* Actualizamos el valor del ECM */
			ecm += (pow (Vc - Ve, 2.0)) / (double) N;
		}
		free (config);
		config = NULL;
		
	} else {
		/* Vamos a aproximar el ECM empleando Montecarlo */
		
		for (i=0 ; i<MONTECARLO_BOUND ; i++) {
			
			/* Guardamos en Xc la media muestral de una
			* configuracion aleatoria de valores de la muestra */
			reset_media_m ();
			reset_var_m ();
			for (j=0 ; j<n ; j++) {
				k = disc_gen_uniforme(0, n);
				media_m (sample[k], j+1);
				Vc  = var_m (sample[k], j+1);
			}
			
			/* Actualizamos el valor del ECM */
			ecm += (pow (Vc - Ve, 2.0)) / (double) MONTECARLO_BOUND;
			/*
			if (!(i%500))
			printf ("loop # %u\tecm == %.4f\n", i, ecm);*/
		}
	}
	
	return ecm;
}
/******************************************************************************/



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
/** ~~~~~~~~~~~~~~~~~~~  SIMULACION  -  ESTADISTICOS  ~~~~~~~~~~~~~~~~~~~~~~~ */
/** ------------------------------------------------------------------------- */



/* Vuelve con el # de intervalo en el que cae el valor Xi
 *
 * PRE: I != NULL
 *	los valores en I están en orden estrictamente creciente
 *
 * ie:	Xj == 0.25		 |
 *	 I == { [0.00 , 0.15),	 |
 *		[0.15 , 0.55),	 |>  find_interval (Xj, I, 4) == 1
 *		[0.55 , 1.12),	 |
 *		[1.12 , 2.00) }  |
 */
static unsigned int find_interval (double Xj, double *I, unsigned int k)
{
	unsigned int i = 0;
	
	assert (I != NULL);
	
	while (i<k-1 && Xj > I[i+1]-DBL_EPSILON)
		i++;
	
	return i;
}


/* Estadístico del test Ji-cuadrado para una muestra 'sample' de 'n' valores
 * Los intervalos de agrupacion de resultados deben estar en el parametro 'I'
 * p[i] == "probabilidad de caer en el intervalo Int(i)"
 *
 * Se define al i-esimo intervalo Int(i) como:
 *	Int(i) = [ I[i] , I[i+1] )
 * y para el ultimo intervalo vale que:
 *	Int(k) = [ I[k] , inifinity )
 *
 * PRE: sample != NULL	&&  n == #(sample)
 *	I != NULL	&&  k == #(I)
 *	p != NULL	&&  k == #(p)
 */
double ji_cuadrado (double *sample, unsigned int n,
		    double *I, unsigned int k, double *p)
{
	double t = 0.0;
	unsigned int i = 0, j = 0;
	unsigned int *N = NULL;
	double npi = 0.0;
	
	assert (sample != NULL);
	assert (I!= NULL);
	
	N = (unsigned int *) calloc (k, sizeof (unsigned int));
	assert (N != NULL);
	
	for (j=0 ; j<n ; j++) {
		i = find_interval (sample[j], I, k);
		N[i]++;
	}
	
	
	for (i=0 ; i<k ; i++) {
		npi = ((double) n) * p[i];
		t += pow (N[i] - npi, 2.0) / npi;
	}
	
	free(N);
	
	return t;
}




/* Compara dos 'doubles' pasados por referencia y devuelve:
 * a > b  =>  1
 * a = b  =>  0
 * a < b  => -1
 */
static int cmp_dbl (const void *a, const void *b)
{
	return  (*((const double *) a) > *((const double *) b)) ?  1 : \
		(*((const double *) a) < *((const double *) b)  ? -1 : 0);
}


/* Estadístico del test Kolmogorov-Smirnov de una muestra 'sample' con 'n' datos
 * 'F' es la función de probabilidad teórica a aplicar sobre los datos
 *
 * PRE: sample != NULL  &&  n == #(sample)
 *	F != NULL
 */
 double kolmogorov_smirnov (double *sample, unsigned int n, double (*F) (double))
{
	double	d = -DBL_MAX,
		j = 0.0,
		m = (double) n,
		Fj = 0.0;
	
	assert (sample != NULL);
	assert (F != NULL);
	
	/* Ordenamos la muestra en orden creciente */
	qsort (sample, (size_t) n, sizeof (double), cmp_dbl);
	
	/* Buscamos el estadístico */
	for (j = 0.0 ; j < m ; j += 1.0) {
		
		Fj = F (sample[(int)j]);
		
		/* Notar que aquí 0 <= j < n, por lo cual usamos j+1 */
		d = MAX (d , (j+1.0)/m - Fj );
		d = MAX (d , Fj - j/m);
	}
		
	return d;
}

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/

/******************************************************************************/



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







