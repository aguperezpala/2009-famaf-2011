/*
 * Adaptive-Network-Based Fuzzy Inference System (ANFIS)
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 13/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "anfis.h"


#define  MAX(x,y)	((x) > (y) ? (x) : (y))
#define  MIN(x,y)	((x) < (y) ? (x) : (y))

#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long)-1)
#define  int_val(x)	((x) & (~(((unsigned long) 1) << MSB)))

/* Debugging printing functions */
#ifdef _DEBUG
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
#else
  #define  debug(s,...)
  #define  dfor(s)
#endif



/**######################    STRUCTURES & STUFF   ############################*/


typedef struct {
	MF	*A;	/* membership functions    (n   in total) */
	double	*P;	/* polynomial coefficients (n+1 in total) */
} branch;

struct _anfis_s {
	long n;		/* # of inputs */
	long t;		/* # of branches */
	branch *b;	/* branches */
};



double triangular (double a, double b, double c, double x);
double triangular (double a, double b, double c, double x)
{ return MAX (0.0, MIN ((x-a)/(b-a), (c-x)/(c-b))); }


double trapezoidal (double a, double b, double c, double d, double x);
double trapezoidal (double a, double b, double c, double d, double x)
{ return MAX (0.0, MIN ( MIN ((x-a)/(b-a), (d-x)/(d-c)), 1.0) ); }


double gaussian (double a, double sigma, double x);
double gaussian (double a, double sigma, double x)
{ return exp (-1.0 * pow (((x-a)/sigma), 2.0)); }


double bellian (double a, double b, double c, double x);
double bellian (double a, double b, double c, double x)
{ return 1.0 / (1.0 + pow (fabs ((x-c)/a), 2.0*b)); }


/**###############    CREATE / DESTROY FUNCTIONS    ##########################*/


/* Creates an instance of the ADT
 *
 * PARAMETERS:	n ---> input dimension
 *		t ---> # of branches the network will have
 *		a ---> membership functions. This must be a vector
 *			of length [t]x[n], simulating a [t][n] matrix
 *
 * PRE: a != NULL
 * POS: result != NULL
 */
anfis_t
anfis_create (unsigned long n, unsigned long t, const MF *a)
{
	anfis_t net = NULL;
	long i = 0, j = 0;
	
	assert (a != NULL);
	
	net = (anfis_t) malloc (sizeof (struct _anfis_s));
	assert (net != NULL);
	
	net->n = int_val(n);
	net->t = int_val(t);
	
	net->b = (branch *) malloc (t * sizeof (branch));
	assert (net->b != NULL);
	
	for (i=0 ; i < t ; i++) {
		net->b[i].A = (MF *) malloc (n * sizeof (MF));
		assert (net->b[i].A != NULL);
		
		net->b[i].P = (double *) calloc (n+1 , sizeof (double));
		assert (net->b[i].P != NULL);
	}
	
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < t*n ; i++) {
		net->b[i/n] . A[i%n] . k = a[i].k;
		for (j=0 ; j < MAX_PARAM ; j++) {
			net->b[i/n] . A[i%n] . p[j] = a[i].p[j];
		}
	}
	
	return net;
}




/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = anfis_destroy (net);
 */
anfis_t
anfis_destroy (anfis_t net)
{
	int i = 0;
	
	assert (net != NULL);
	
	for (i=0 ; i < net->t ; i++) {
		free (net->b[i].A);	net->b[i].A = NULL;
		free (net->b[i].P);	net->b[i].P = NULL;
	}
	free (net->b);	net->b = NULL;
	free (net);		net = NULL;
	
	return net;
}




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


/* Prints in STDOUT the internal state of the network
 * PRE: net != NULL
 */
void
anfis_print (anfis_t net)
{
	int i = 0, j = 0;
	char MF_kinds[MAX_FUNC][25] = {"triang", "trap", "gauss", "bell"};
	
	assert (net != NULL);
	
	printf ("# of inputs:   N = %ld\n# of branches: T = %ld", net->n, net->t);
	
	for (i=0 ; i < net->t ; i++) {
		
		printf ("\n\nBranch # %d\n", i);
		
		for (j=0 ; j < net->n ; j++) {
			
			printf ("MF[%d] kind: %s\nParamaters: ",
				j, MF_kinds[net->b[i].A[j].k]);
		
			switch (net->b[i].A[j].k) {
			
			case (triang):
				printf ("a = %.4f, b = %.4f, c = %.4f\n", 
					net->b[i].A[j].p[0],
					net->b[i].A[j].p[1],
					net->b[i].A[j].p[2] );
				break;
				
			case (trap):
				printf ("a = %.4f, b = %.4f, c = %.4f, d = %.4f\n", 
					net->b[i].A[j].p[0],
					net->b[i].A[j].p[1],
					net->b[i].A[j].p[2],
					net->b[i].A[j].p[3] );
				break;
				
			case (gauss):
				printf ("a = %.4f, σ = %.4f\n", 
					net->b[i].A[j].p[0],
					net->b[i].A[j].p[1] );
				break;
				
			case (bell):
				printf ("a = %.4f, b = %.4f, c = %.4f\n", 
					net->b[i].A[j].p[0],
					net->b[i].A[j].p[1],
					net->b[i].A[j].p[2] );
				break;
				
			default:
				printf ("Error: unknown membership function\n");
				break;
			}
		}
		
		printf ("Consequent parameters:\n");
		for (j=0 ; j <= net->n ; j++) {
			printf ("p[%d][%d] = %.2f\t", i, j, net->b[i].P[j]);
		}
	}
	printf ("\n\n");
	
	return;
}

