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
#include <string.h>
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


char MF_kinds[MAX_FUNC][MAX_NLEN] = {
	"triang",
	"trap",
	"gauss",
	"bell"
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


double belly (double a, double b, double c, double x);
double belly (double a, double b, double c, double x)
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




/* Prints into STDOUT the internal state of the network's i-th branch
 * PRE: net != NULL
 *	i < network # of branches
 */
void
anfis_print_branch (anfis_t net, unsigned int i)
{
	int j = 0;
	
	assert (net != NULL);
	assert (i < (unsigned long) net->t);
	
	printf ("\nBranch # %d\n", i);
	
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




/* Prints in STDOUT the internal state of the network
 * PRE: net != NULL
 */
void
anfis_print (anfis_t net)
{
	int i = 0;
	
	assert (net != NULL);
	
	printf ("# of inputs:   N = %ld\n# of branches: T = %ld", net->n, net->t);
	
	for (i=0 ; i < net->t ; i++) {
		printf ("\n");
		anfis_print_branch (net, i);
	}
	printf ("\n\n");
	
	return;
}




/* Copies into 'a' the network's i-th row membership function,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE: net != NULL
 *	a   != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] data has been copied inside 'a'
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_get_MF (anfis_t net, unsigned int i, unsigned int j, MF *a)
{
	int l = 0;
	
	assert (net != NULL);
	assert (a   != NULL);
	assert (i < (unsigned long) net->t);
	assert (j < (unsigned long) net->n);
	
	a->k = net->b[i].A[j].k;
	for (l=0 ; l < MAX_PARAM ; l++) {
		a->p[l] = net->b[i].A[j].p[l];
	}
	
	return ANFIS_OK;
}




/* Sets 'a' as the new network membership function in the i-th row,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE:	net != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] has been replaced with 'a'
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_set_MF (anfis_t net, unsigned int i, unsigned int j, MF a)
{
	int l = 0;
	
	assert (net != NULL);
	assert (i < (unsigned long) net->t);
	assert (j < (unsigned long) net->n);
	
	net->b[i].A[j].k = a.k;
	for (l=0 ; l < MAX_PARAM ; l++) {
		net->b[i].A[j].p[l] = a.p[l];
	}
	
	return ANFIS_OK;
}




/* Returns a copy of the network's i-th consequent parameters
 * Caller owns the memory generated for the returned vector,
 * which must be freed using glibc standard free() routine
 *
 * PRE:	net   != NULL
 *	i < network # of branches
 *	
 * POS: result != NULL   &&   result contains the coefficients of p[i]
 *	or
 *	result == NULL
 */
double *
anfis_get_P (anfis_t net, unsigned int i)
{
	double *p = NULL;
	
	assert (net != NULL);
	assert (i < (unsigned long) net->t);
	
	p = (double *) malloc ((net->n+1) * sizeof (double));
	if (p != NULL) {
		p = memcpy (p, net->b[i].P, (net->n + 1) * sizeof (double));
	}
	
	return p;
}




/* Sets 'new_p' as the new network i-th row consequent parameters (aka: p[i])
 *
 * PRE:	net   != NULL
 *	new_p != NULL
 *	length_of (new_p) == network input dimension + 1
 *	i < network # of branches
 *	
 * POS: result == ANFIS_OK   &&   coefficients in p[i] have been replaced
 *				  with those in 'new_p'
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_set_P (anfis_t net, unsigned int i, const double *new_p)
{
	assert (net   != NULL);
	assert (new_p != NULL);
	assert (i < (unsigned long) net->t);
	
	net->b[i].P = memcpy (net->b[i].P, new_p, (net->n+1) * sizeof (double));
	
	return ANFIS_OK;
}
