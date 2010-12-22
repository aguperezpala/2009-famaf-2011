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
#include <gsl/gsl_multifit.h>

#include "anfis.h"


#define  MAX(x,y)	((x) > (y) ? (x) : (y))
#define  MIN(x,y)	((x) < (y) ? (x) : (y))

#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long)-1)
#define  int_val(x)	((x) & (~(((unsigned long) 1) << MSB)))

/* Debugging printing functions */
#ifdef _DEBUG
  #define  handle_error_1(err)   if ((err) != ANFIS_OK) return err;
  #define  handle_error_2(arr)   if ((arr) == NULL) return ANFIS_ERR;
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
#else
  #define  handle_error_1(err)
  #define  handle_error_2(arr)
  #define  debug(s,...)
  #define  dfor(s)
#endif




/**######################    STRUCTURES & STUFF   ############################*/


typedef struct {
	MF_t	   *MF;	/* membership functions    (n   in total) */
	gsl_vector *P;	/* polynomial coefficients (n+1 in total) */
} branch;

struct _anfis_s {
	long n;		/* # of inputs */
	long t;		/* # of branches */
	branch *b;	/* branches */
	double *tau;	/* auxiliary values for signal propagation */
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
 *		mf --> membership functions. This must be a vector
 *			of length [t]x[n], simulating a [t][n] matrix
 *
 * PRE: mf != NULL
 * POS: result != NULL
 */
anfis_t
anfis_create (unsigned long n, unsigned long t, const MF_t *mf)
{
	anfis_t net = NULL;
	long i = 0, j = 0;
	
	assert (mf != NULL);
	
	net = (anfis_t) malloc (sizeof (struct _anfis_s));
	assert (net != NULL);
	
	net->n = int_val(n);
	net->t = int_val(t);
	
	net->b = (branch *) malloc (t * sizeof (branch));
	assert (net->b != NULL);
	
	for (i=0 ; i < t ; i++) {
		net->b[i].MF = (MF_t *) malloc (n * sizeof (MF_t));
		assert (net->b[i].MF != NULL);
		
		net->b[i].P = gsl_vector_alloc (n+1);
		assert (net->b[i].P != NULL);
	}
	
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < t*n ; i++) {
		net->b[i/n] . MF[i%n] . k = mf[i].k;
		for (j=0 ; j < MAX_PARAM ; j++) {
			net->b[i/n] . MF[i%n] . p[j] = mf[i].p[j];
		}
	}
	
	net->tau = (double *) malloc (net->t * sizeof (double));
	assert (net->tau != NULL);
	
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
		free (net->b[i].MF);
		net->b[i].MF = NULL;
		gsl_vector_free (net->b[i].P);
		net->b[i].P  = NULL;
	}
	free (net->b);		net->b     = NULL;
	free (net->tau);	net->tau   = NULL;
	free (net);		net        = NULL;
	
	return net;
}




/**#####################    ACCESSORS / OBSERVERS    #########################*/


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
			j, MF_kinds[net->b[i].MF[j].k]);
	
		switch (net->b[i].MF[j].k) {
		
		case (triang):
			printf ("a = %.4f, b = %.4f, c = %.4f\n", 
				net->b[i].MF[j].p[0],
				net->b[i].MF[j].p[1],
				net->b[i].MF[j].p[2] );
			break;
			
		case (trap):
			printf ("a = %.4f, b = %.4f, c = %.4f, d = %.4f\n", 
				net->b[i].MF[j].p[0],
				net->b[i].MF[j].p[1],
				net->b[i].MF[j].p[2],
				net->b[i].MF[j].p[3] );
			break;
			
		case (gauss):
			printf ("a = %.4f, σ = %.4f\n", 
				net->b[i].MF[j].p[0],
				net->b[i].MF[j].p[1] );
			break;
			
		case (bell):
			printf ("a = %.4f, b = %.4f, c = %.4f\n", 
				net->b[i].MF[j].p[0],
				net->b[i].MF[j].p[1],
				net->b[i].MF[j].p[2] );
			break;
			
		default:
			printf ("Error: unknown membership function\n");
			break;
		}
	}
	
	printf ("Consequent parameters:\n");
	for (j=0 ; j <= net->n ; j++) {
		printf ("p[%d][%d] = %.2f\t", i, j,
			gsl_vector_get (net->b[i].P, j));
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




/* Copies into 'mf' the network's i-th row membership function,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE: net != NULL
 *	mf  != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] data has been copied inside 'mf'
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_get_MF (anfis_t net, unsigned int i, unsigned int j, MF_t *mf)
{
	int l = 0;
	
	assert (net != NULL);
	assert (mf  != NULL);
	assert (i < (unsigned long) net->t);
	assert (j < (unsigned long) net->n);
	
	mf->k = net->b[i].MF[j].k;
	for (l=0 ; l < MAX_PARAM ; l++) {
		mf->p[l] = net->b[i].MF[j].p[l];
	}
	
	return ANFIS_OK;
}




/* Sets 'mf' as the new network membership function in the i-th row,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE:	net != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] has been replaced with 'mf'
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_set_MF (anfis_t net, unsigned int i, unsigned int j, MF_t mf)
{
	int l = 0;
	
	assert (net != NULL);
	assert (i < (unsigned long) net->t);
	assert (j < (unsigned long) net->n);
	
	net->b[i].MF[j].k = mf.k;
	for (l=0 ; l < MAX_PARAM ; l++) {
		net->b[i].MF[j].p[l] = mf.p[l];
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
	int j = 0;
	double *p = NULL;
	
	assert (net != NULL);
	assert (i < (unsigned long) net->t);
	
	p = (double *) malloc ((net->n+1) * sizeof (double));
	if (p != NULL) {
		for (j=0 ; j < net->n+1 ; j++) {
			p[j] = gsl_vector_get (net->b[i].P, j);
		}
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
	int j = 0;
	
	assert (net   != NULL);
	assert (new_p != NULL);
	assert (i < (unsigned long) net->t);
	
	for (j=0 ; j < net->n+1 ; j++) {
		gsl_vector_set (net->b[i].P, j, new_p[j]);
	}
	
	return ANFIS_OK;
}




/**#######################    NETWORK DINAMICS    ############################*/


/* Evaluates the specified membership function in the given 'x' value */
static double
eval_MF (MF_t mf, double x)
{
	switch (mf.k) {
	
	case (triang):
		return triangular (mf.p[0], mf.p[1], mf.p[2], x);
		break;
		
	case (trap):
		return trapezoidal (mf.p[0], mf.p[1], mf.p[2], mf.p[3], x);
		break;
		
	case (gauss):
		return gaussian (mf.p[0], mf.p[1], x);
		break;
		
	case (bell):
		return belly (mf.p[0], mf.p[1], mf.p[2], x);
		break;
		
	default:
		fprintf (stderr, "Error: unknown membership function\n");
		return 0.0;
		break;
	}
}




/* Evaluates the network's membership functions in the given input
 * Obtained membership values are stored in the k-th row of MF matrix
 *
 * PRE: net   != NULL
 *	input != NULL
 *	MF    != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == ANFIS_OK   &&   membership values stored in MF
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_compute_membership (const anfis_t net, gsl_vector *input,
			  gsl_matrix *MF, int k)
{
	int i = 0, n = 0, t = 0;
	double value = 0.0;
	
	assert (net   != NULL);
	assert (input != NULL);
	assert (MF    != NULL);
	
	t = net->t;
	n = net->n;
	
	#pragma omp parallel for shared(MF,input)
	for (i=0 ; i < t*n ; i++) {
		value = eval_MF (net->b[i/n].MF[i%n],
				 gsl_vector_get (input, i%n));
		gsl_matrix_set (MF, k, i, value);
	}
	
	return ANFIS_OK;
}




/* Performs a partial forward propagation of the given input to compute
 * the barred-taus, which are then stored in b_tau k-th row
 *
 * MF k-th row must contain the network's membership values for this input
 *
 * PRE: net   != NULL
 *	input != NULL
 *	MF    != NULL
 *	b_tau != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == ANFIS_OK   &&   b_tau values in net were updated
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_partial_fwd_prop (anfis_t net, gsl_vector *input, const gsl_matrix *MF,
			gsl_matrix *b_tau, int k)
{
	int i = 0, j = 0, t = 0, n = 0;
	double *tau = NULL, tau_sum = 0.0;
	
	assert (net   != NULL);
	assert (input != NULL);
	assert (MF    != NULL);
	
	t     = net->t;
	n     = net->n;
	tau   = net->tau;
	
	/* First taus are computed */
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < t ; i++) {
		tau[i] = 1.0;
		for (j=0 ; j < n ; j++) {
			tau[i] *= gsl_matrix_get (MF, k, i*n+j);
		}
	}
	
	/* Now we calculate b_taus */
	#pragma omp parallel for default(shared) private(i) reduction(+:tau_sum)
	for (i=0 ; i < t ; i++) {
		tau_sum += tau[i];
	}
	#pragma omp parallel for default(shared) private(i)
	for (i=0 ; i < t ; i++) {
		gsl_matrix_set (b_tau, k, i, tau[i] / tau_sum);
	}
	
	tau   = NULL;
	
	return ANFIS_OK;
}




/* Performs least square estimate (aka LSE) for the given sample (of size P)
 * to find best values for the network's consequent parameters
 *
 * PRE: net != NULL
 *	A   != NULL
 *	s   != NULL
 *
 * POS: result == ANFIS_OK   &&   consequent parameters computed and stored
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_lse (anfis_t net, const gsl_matrix *A, const t_sample *s, unsigned int P)
{
	gsl_multifit_linear_workspace *work = NULL;
	gsl_matrix *cov = NULL;	/* Covariance matrix generated for LSE */
	gsl_vector *y = NULL,	/* Sample results (P in total) */
		   *p = NULL;	/* Estimated parameters (M in total) */
	size_t M = 0;
	unsigned int i = 0;
	double err = 0.0;
	
	assert (net != NULL);
	assert (A   != NULL);
	assert (s   != NULL);
	
	M = ((size_t) net->t) * ((size_t) net->n + 1);
	
	/* Saving desired results in vector 'y' */
	y = gsl_vector_alloc (P);
	handle_error_2 (y);
	for (i=0 ; i < P ; i++) {
		gsl_vector_set (y, i, s[i].out);
	}
	
	/* Generating necessary workspace */
	p = gsl_vector_alloc (M);
	handle_error_2 (p);
	cov = gsl_matrix_alloc (M, M);
	handle_error_2 (cov);
	work = gsl_multifit_linear_alloc ((size_t) P, M);
	handle_error_2 (work);
	
	/* Performing LSE */
	gsl_multifit_linear (A, y, p, cov, &err, work);
	
	/* Hastly freeing the insulting amount of memory reserved */
	gsl_multifit_linear_free (work);
	work = NULL;
	gsl_matrix_free (cov);
	cov = NULL;
	gsl_vector_free (y);
	y = NULL;
	
	/* Saving estimated parameters inside the network */
	for (i=0 ; i < net->t ; i++) {
		/* p_sub references the i-th branch consequent parameters
		 * estimation, originally stored in vector p */
		gsl_vector_view p_sub;
		
		p_sub = gsl_vector_subvector (p, i*(net->n+1), net->n+1);
		gsl_vector_memcpy (net->b[i].P, &p_sub.vector);
	}
	
	gsl_vector_free (p);
	p = NULL;
	
	return ANFIS_OK;
}




/* Trains the network using the 'P' training samples provided
 * Input of every sample element should have the correct dimension
 * P should overcome the dimension of the input.
 *
 * NOTE: batch-like update mode is used
 * WARNING: this routine is EXTREMELY SLOW & CPU BOUND
 *
 * PRE: net != NULL
 *	s   != NULL
 *	length_of (s) == P
 *	length_of (s[k].in) == network input dimension      k ∈ {1,..,P}
 *	P > network input dimension
 *
 * POS:	result == ANFIS_OK   &&   net's parameters have been updated
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_train (anfis_t net, unsigned int P, const t_sample *s)
{
	int res = ANFIS_OK;
	long k = 0, i = 0;
	size_t n = 0, t = 0, M = 0;
	gsl_matrix  *MF = NULL,		/* Membership values for input */
		    *b_tau = NULL,	/* Barred taus for input */
		    *A  = NULL;		/* Freaking monster */
	double value = 0.0;
	
	assert (net != NULL);
	assert (s   != NULL);
	assert (P > net->n);
	
	t = (size_t) net->t;
	n = (size_t) net->n;
	M = t * (n+1);
	
	/* Membership values for all the P inputs */
	MF = gsl_matrix_alloc (P, t*n);
	handle_error_2 (MF);
	
	/* Barred taus for all the P inputs */
	b_tau = gsl_matrix_alloc (P, t);
	handle_error_2 (b_tau);
/*
A[1,1]=b_tau[1] | A[1,2]=b_tau[1]*input[1,1] | ... | A[1,M]=b_tau[t]*input[1,n]
A[2,1]=b_tau[1] | A[2,2]=b_tau[1]*input[2,1] | ... | A[1,M]=b_tau[t]*input[2,n]
      ...       |            ...             | ... |            ...
A[P,1]=b_tau[1] | A[P,2]=b_tau[1]*input[P,1] | ... | A[1,M]=b_tau[t]*input[P,n]
*/	A = gsl_matrix_alloc (P, M);
	handle_error_2 (A);
	
	
	/* Performing P partial propagations to compute A matrix */
	for (k=0 ; k < P ; k++) {
		
		/* Filling MF matrix k-th row */
		res = anfis_compute_membership (net, s[k].in, MF, k);
		handle_error_1 (res);
		
		/* Filling b_tau matrix k-th row */
		res = anfis_partial_fwd_prop (net, s[k].in, MF, b_tau, k);
		handle_error_1 (res);
		
		/* Filling A matrix k-th row */
		#pragma omp parallel for default(shared) private(i,value)
		for (i=0 ; i < M ; i++) {
			
			value = gsl_matrix_get (b_tau, k, i/(n+1));
			if (i % (n+1))
				value *= gsl_vector_get (s[k].in, (i%(n+1))-1);
			
			gsl_matrix_set (A, k, i, value);
		}
	}
	
	res = anfis_lse (net, A, s, P);
	handle_error_1 (res);
	
	/** TODO: gradient descent method to update MF[i][j] parameters **/
	
	return res;
}




/* Feeds the network with the given input. Returns the network produced output
 *
 * PRE: net != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == network output for the given input
 */
double
anfis_eval (anfis_t net, double *input)
{
	/** TODO: this function... */
	return 0.0;
}

