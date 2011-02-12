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
#include <float.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_errno.h>
#include <omp.h>

#include "anfis.h"


/** NOTE <Heuristic for etha modification>
 **	 a) if we have INC_H consecutive error minimizations --> etha increases
 **	 b) if we have DEC_H succesions of 1 up, 1 down -------> etha decreases
 **/


#define  _gamma		(1<<15)

/* Initial value for network's etha */
#define  INIT_ETHA	1.0
/* Etha's increase/decrease heuristic codes */
#define  FIXED		-1
#define  DECREASE	0
#define  INCREASE	1
/* Etha's increase/decrease heuristic thresholds */
#define  DEC_H		2
#define  INC_H		4
/* Etha's increase/decrease heuristic coefficients */
#define  DEC_C		0.1
#define  INC_C		0.1

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
  #define  handle_error_3(arr)   if ((arr) == NULL) return NULL;
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
#else
  #define  handle_error_1(err)
  #define  handle_error_2(arr)
  #define  handle_error_3(arr)
  #define  debug(s,...)
  #define  dfor(s)
#endif




/**######################    STRUCTURES & STUFF   ############################*/


/* A single network branch. That includes: ... */
typedef struct {
	MF_t	   *MF;	/* ... membership functions    (n   in total) */
	gsl_vector *P;	/* ... consequent coefficients (n+1 in total) */
} branch;


/* Network internal structure */
struct _anfis_s {
	long n;		/* # of inputs */
	long t;		/* # of branches */
	double etha;	/* update step size */
	
	branch *b;	/* branches */
	double *tau;	/* auxiliary values for signal propagation */
	
	double old_err1;/* learning error from the previous epoch  */
	double old_err2;/* learning error from two previous epochs */
	int trend;	/* etha heuristic state: 0 = decrease ; 1 = increase  */
	int trend_st;	/* trend state */
};



/* Internal TAD for computing the premises parameters update
 * (ie: MF internal parameters) using the gradient descent method
 */
typedef struct {
	const t_sample *s;  /* samples training set for this epoch */
	MF_t *mf_ij;	    /* MF whose parameters must be updated */
	gsl_vector *db_e_ij;/* error gradient for each parameter of this MF */
	gsl_vector *MF_val; /* MF values for all P inputs in the given sample */
	gsl_vector *b_tau;  /* barred tau values (in MF's branch) of sample */
	gsl_vector *p_sub;  /* consequent parameters (in MF's branch) */
	gsl_vector *f_out;  /* network output values for the given sample */
} mfu_t; 


/* For pretty printing */
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
{ return (1.0 / (1.0 + pow (fabs ((x-c)/a), 2.0*b))); }




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
	
	net->n    = int_val(n);
	net->t    = int_val(t);
	net->etha = INIT_ETHA;
	net->old_err1 = 0.0;
	net->old_err2 = 0.0;
	net->trend    = FIXED;
	net->trend_st = 0;
	
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
		fprintf (stderr, "anfis.c: eval_MF: ERROR: unknown MF\n");
		return 0.0;
		break;
	}
}




/* Evaluates the network's membership functions in the given input
 * Obtained membership values are stored in vector MF_k
 *
 * PRE: net   != NULL
 *	input != NULL
 *	MF_k  != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == ANFIS_OK   &&   membership values stored in MF
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_compute_membership (const anfis_t net, const gsl_vector *input,
			  gsl_vector *MF_k)
{
	int i = 0, n = 0, t = 0;
	double value = 0.0;
	
	assert (net   != NULL);
	assert (input != NULL);
	assert (MF_k  != NULL);
	
	t = net->t;
	n = net->n;
	
	#pragma omp parallel for shared(MF_k,input) private(value)
	for (i=0 ; i < t*n ; i++) {
		value = eval_MF (net->b[i/n].MF[i%n],
				 gsl_vector_get (input, i%n));
		assert (value >= 0.0);
		gsl_vector_set (MF_k, i, value);
	}
	
	return ANFIS_OK;
}




/* Performs a partial forward propagation of the given input to compute
 * the barred-taus, which are then stored in b_tau_k
 *
 * MF_k must contain the network's membership values for this input
 *
 * PRE: net     != NULL
 *	input   != NULL
 *	MF_k    != NULL
 *	b_tau_k != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == ANFIS_OK   &&   b_tau_k values in net were updated
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_partial_fwd_prop (const anfis_t net, const gsl_vector *input,
			const gsl_vector *MF_k,  gsl_vector *b_tau_k)
{
	int i = 0, j = 0, t = 0, n = 0;
	double *tau = NULL, tau_sum = 0.0;
	
	assert (net     != NULL);
	assert (input   != NULL);
	assert (MF_k    != NULL);
	assert (b_tau_k != NULL);
	
	t   = net->t;
	n   = net->n;
	tau = net->tau;
	
	/* First taus are computed */
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < t ; i++) {
		tau[i] = 1.0;
		for (j=0 ; j < n ; j++) {
			tau[i] *= gsl_vector_get (MF_k, i*n+j);
		}
		assert (tau[i] >= 0.0);
	}
	
	/* Now we calculate b_taus */
	tau_sum = 0.0;
	#pragma omp parallel for shared(tau) reduction(+:tau_sum)
	for (i=0 ; i < t ; i++) {
		tau_sum += tau[i];
	}
	#pragma omp parallel for default(shared) private(i)
	for (i=0 ; i < t ; i++) {
		if (tau_sum != 0.0) {
			gsl_vector_set (b_tau_k, i, tau[i] / tau_sum);
		} else {
			gsl_vector_set (b_tau_k, i, 0.0);
		}
	}
	
	tau = NULL;
	
	return ANFIS_OK;
}




/* Snew = S * A_i * At_i * S */
static void
calc_num (const gsl_matrix *S, const gsl_vector *A_i, gsl_matrix *Snew,
	  gsl_matrix *m_aux, gsl_vector *v_aux, size_t M)
{
	int i = 0;
	double value = 0.0;
	
	/* v_aux = S * A_i */
	gsl_blas_dgemv (CblasNoTrans, 1.0, S, A_i, 0.0, v_aux);
	
	/* m_aux = v_aux * At_i */
	#pragma omp parallel for default(shared) private(i,value)
	for (i=0 ; i < M*M ; i++) {
		value = gsl_vector_get (v_aux, i/M) * gsl_vector_get (A_i, i%M);
		gsl_matrix_set (m_aux, i/M, i%M, value);
	}
	
	/* Snew = m_aux * S */
	gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, m_aux, S, 0.0, Snew);
	
	return;
}




/* Returns the value: At_i * S * A_i */
static double
calc_den (const gsl_matrix *S, const gsl_vector *A_i, gsl_vector *v_aux)
{
	double res = 0.0;
	
	/* v_aux = S * A_i */
	gsl_blas_dgemv (CblasNoTrans, 1.0, S, A_i, 0.0, v_aux);
	
	/* res = At_i * v_aux */
	gsl_blas_ddot (A_i, v_aux, &res);
	
	return res;
}




/* Performs LSE of consequent parameters for all branches in the network
 *
 * PARAMETERS:	A ---> predictor variables matrix
 *		y ---> expected results for this sample set  (P in total)
 *
 * PRE: A != NULL
 *	y != NULL
 *
 * POS: result != NULL   &&   best fit consequent parameters vector returned
 *	or
 *	result == NULL
 */
static gsl_vector *
anfis_fit_linear (const gsl_matrix *A, const gsl_vector *y, size_t P, size_t M)
{
	gsl_matrix  *S =    NULL,	/* Covariance matrix */
		    *Snew = NULL,
		    *Saux = NULL;
	gsl_vector  *X = NULL,		/* Future best fit parameters */
		    *Xnew = NULL;
	unsigned int i = 0;
	double den = 0.0, factor = 0.0;
	
	assert (A != NULL);
	assert (y != NULL);
	
	/* Generating necessary workspace */
	S = gsl_matrix_alloc (M,M);
	if (S == NULL)
		goto exit_point;
	Snew = gsl_matrix_calloc (M,M);
	if (Snew == NULL)
		goto exit_point;
	Saux = gsl_matrix_calloc (M,M);
	if (Saux == NULL)
		goto exit_point;
	Xnew = gsl_vector_alloc (M);
	if (Xnew == NULL)
		goto exit_point;
	X = gsl_vector_calloc (M);
	if (X == NULL)
		goto exit_point;
	
	/* S = γ*Id , where γ is a large number */
	gsl_matrix_set_identity  (S);
	gsl_matrix_scale (S, _gamma);
	
	/* Performing Least Square Estimation */
	for (i=0 ; i < P ; i++) {
		/* Matrix A i-th row (row At_i+1 in Jang's paper) */
		gsl_vector_const_view A_i = gsl_matrix_const_row (A, i);
		
		/* Snew = S(i) * A_i+1 * At_i+1 * S(i) */
		calc_num (S, &(A_i.vector), Snew, Saux, Xnew, M);
		/* scale = At_i+1 * S(i) * A_i+1 */
		den = calc_den (S, &(A_i.vector), Xnew);
		/* Snew = Snew / (1+scale) */
		gsl_matrix_scale (Snew, 1.0 / (1.0+den));
		/* S(i+1) = S(i) - Snew */
		gsl_matrix_sub (S, Snew);
		
		/* factor = At_i+1 * X(i) */
		gsl_blas_ddot (&(A_i.vector), X, &factor);
		/* factor = yt_i+1 - factor */
		factor = gsl_vector_get (y, i) - factor;
		/* Xnew = S(i+1) * A_i+1 */
		gsl_blas_dgemv (CblasNoTrans, 1.0, S, &(A_i.vector), 0.0, Xnew);
		/* Xnew = Xnew * factor */
		gsl_vector_scale (Xnew, factor);
		/* X(i+1) = X(i) + Xnew */
		gsl_vector_add (X, Xnew);
	}
	
	exit_point:
		if (S != NULL)    { gsl_matrix_free (S);    S    = NULL;}
		if (Snew != NULL) { gsl_matrix_free (Snew); Snew = NULL;}
		if (Saux != NULL) { gsl_matrix_free (Saux); Saux = NULL;}
		if (Xnew != NULL) { gsl_vector_free (Xnew); Xnew = NULL;}
		
		return X;
}




/* Performs least square estimate (aka LSE) for the given sample (of size P)
 * to find best values for the network's consequent parameters
 *
 * The network is updated internally, and a copy of these new values is returned
 * as a vector of length (network # of branches) * (network input dimension + 1)
 * to facilitate further calculations. Free result using gsl_vector_free()
 *
 * PRE: net != NULL
 *	A   != NULL
 *	s   != NULL
 *
 * USE: new_params = anfis_lse
 *
 * POS: new_params != NULL   &&   consequent parameters were computed and saved
 *				  in net. A copy of the new values was returned
 *	or
 *	new_params == NULL
 */
static gsl_vector *
anfis_lse (anfis_t net, const gsl_matrix *A, const t_sample *s, unsigned int P)
{
	size_t M = 0;
	gsl_vector *y = NULL,	 /* Sample results       (P in total) */
		   *X = NULL;	 /* Estimated parameters (M in total) */
	unsigned int i = 0, j = 0;
	static size_t calls = 0;
	
	assert (net != NULL);
	assert (A   != NULL);
	assert (s   != NULL);
	
	M = ((size_t) net->t) * ((size_t) net->n + 1);
	
	/* Saving desired results in vector 'y' */
	y = gsl_vector_alloc (P);
	handle_error_3 (y);
	for (i=0 ; i < P ; i++) {
		gsl_vector_set (y, i, s[i].out);
	}
	
	X = anfis_fit_linear (A, y, P, M);
	
	if (X != NULL) {
		/* Saving estimated parameters inside the network */
/*		debug ("LSE parameters estimation (call # %lu):\n", ++calls);
*/		for (i=0 ; i < net->t ; i++) {
			/* p_sub references the i-th branch consequent param.
			 * estimation, originally stored in vector X */
			gsl_vector_view p_sub;
			
			p_sub = gsl_vector_subvector (X, i*(net->n+1), net->n+1);
			gsl_vector_memcpy (net->b[i].P, &(p_sub.vector));
			
/*			dfor (j=0 ; j <= net->n ; j++) {
				debug ("p[%u][%u] = %.4f\n", i, j,
				gsl_vector_get (X, i*(net->n+1) + j));
			}
*/		}
		j=j;calls=calls;
	}
	
	gsl_vector_free (y);
	y = NULL;
	
	return X;
}




/* Computes error gradient for all parameters of the given membership function.
 * Each parameter's err.grad. is stored in vector db_e_ij.
 * The whole learning error is the returned value.
 *
 * PARAMETERS:  mfu --> MF update TAD
 *		P ----> # of samples in the set
 *		j ----> order # of the input element this MF processes
 *		n ----> network input dimension
 *
 * PARAMETERS INSIDE mfu:
 *		s --------> samples training set
 *		mf_ij ----> membership function whose parameters must be updated
 *		db_e_ij --> error gradient for each parameter of this MF
 *		MF_val ---> MF values for all P inputs in the given sample
 *		b_tau ----> barred tau values for all P inputs (for this branch)
 *		p_sub ----> consequent parameters (for this branch)
 *		f_out ----> network output values for the given sample
 *		
 * PRE: mfu != NULL
 *	All values inside mfu != NULL
 *	All vectors dimension (inside mfu) == P
 *	j < n
 *
 * USE: err = anfis_update_mf (&mfu, db_e_ij, j, P, n)
 *
 * POS: !isnan(err)   &&   gradients stored in db_e_ij
 *		      &&   learning error returned
 *	or
 *	isnan(err)   &&   an internal misfunction has ocurred
 */
static double
anfis_calc_mf_up (mfu_t *mfu, unsigned int j, unsigned int P, unsigned int n)
{
	int k = 0;
	MF_t *mf_ij = NULL;	/* MF whose parameters must be updated */
	gsl_vector *db_e_ij = NULL;	/* error gradient for MF's parameters */
	double	mf_k = 0.0,	/* MF value for the k-th sample */
		w    = 0.0,	/* mf_k * (1.0 - mf_k) */
		xj   = 0.0,	/* j-th input element for k-th sample */
		fk   = 0.0,	/* network output for k-th sample */
		pk   = 0.0,
		tk   = 0.0,	/* barred tau for k-th sample (of this branch) */
		yk   = 0.0,	/* correct output for k-th sample */
		aux  = 0.0;
	double  d_mf[MAX_PARAM];  /* derivatives of MF to its parameters */
	
	
	assert (mfu != NULL);
	assert (mfu->mf_ij  != NULL);
	assert (mfu->s      != NULL);
	assert (mfu->MF_val != NULL);
	assert (mfu->b_tau  != NULL);
	assert (mfu->p_sub  != NULL);
	assert (mfu->f_out  != NULL);
	assert (j < n);
	
	mf_ij   = mfu -> mf_ij;
	db_e_ij = mfu -> db_e_ij;
	
	/** TODO: erase the following check and implement support for
	 **	  every membership function defined in the header file
	 **/
	if (mf_ij -> k != bell) {
		fprintf (stderr, "anfis.c: anfis_calc_mf_up: "
				 "Membership function not implemented  ~  "
				 "ABORTING UPDATE !!!\n\n");
		return NAN;
	}
	
	gsl_vector_set_zero (db_e_ij);
	omp_set_nested (1);
	/** WARNING Following loop works only for MF of kind 'bell' */
/*	#pragma omp parallel for default(shared) \
				 private(k, mf_k, w, xj, fk, pk, tk, yk, d_mf, aux)
*/	for (k=0 ; (unsigned int) k < P ; k++) {
		gsl_vector_view  p_aux;
		
		/* mf_k = MF value for k-th sample */
		mf_k = gsl_vector_get (mfu -> MF_val, k);
		w    = mf_k * (1.0 - mf_k);
		
		/* xj = j-th input element (of k-th input) */
		xj = gsl_vector_get ((mfu -> s)[k].in, j);
		
		/* fk = network output for k-th sample */
		fk = gsl_vector_get (mfu -> f_out, k);
		
		pk = 0.0;
		/* pk = p[i][0] + p[i][1] * x[k][1] + ... + p[i][n-1] * x[k][n-1] */
		p_aux = gsl_vector_subvector (mfu -> p_sub, 1, n);
		gsl_blas_ddot (&(p_aux.vector), (mfu -> s)[k].in, &pk);
		pk += gsl_vector_get (mfu -> p_sub, 0);
		
		/* tk = barred tau for k-th sample (of this branch) */
		tk = gsl_vector_get (mfu -> b_tau, k);
		
		/* yk = correct output for k-th sample */
		yk = (mfu -> s)[k].out;
		
		#pragma omp sections private(aux)
		{
			/* Update of first param ('a') */
			#pragma omp section
			{
				d_mf[0] = (-2.0 * w * mf_ij->p[1]) / mf_ij->p[0];
				aux = gsl_vector_get (db_e_ij, 0);
				aux += -2.0 * (yk - fk) * (pk - fk) * tk
					    * (1.0/mf_k) * d_mf[0];
				gsl_vector_set (db_e_ij, 0, aux);
			}
			
			/* Update of second param ('b') */
			#pragma omp section
			if (xj != mf_ij->p[2]) {
				aux = (xj - mf_ij->p[2]) / mf_ij->p[0];
				d_mf[1]  = -2.0 * w * log (fabs (aux));
				aux = gsl_vector_get (db_e_ij, 1);
				aux += -2.0 * (yk - fk) * (pk - fk) * tk
					    * (1.0/mf_k) * d_mf[1];
				gsl_vector_set (db_e_ij, 1, aux);
			}
			
			/* Update of third param ('c') */
			#pragma omp section
			if (xj != mf_ij->p[2]) {
				d_mf[2]  = (-2.0 * w * mf_ij->p[1]) /
						(xj - mf_ij->p[2]);
				aux = gsl_vector_get (db_e_ij, 2);
				aux += -2.0 * (yk - fk) * (pk - fk) * tk
					    * (1.0/mf_k) * d_mf[2];
				gsl_vector_set (db_e_ij, 2, aux);
			}
		}
	}
	omp_set_nested (0);
	
	aux = 0.0;
	for (int l = 0 ; l < MAX_PARAM ; l++) {
		aux += gsl_vector_get (db_e_ij, l);
	}
	
	mf_ij = NULL;
	db_e_ij = NULL;
	
	return aux;
}




/* Modifies network's etha (ie: the update step size) according to a certain
 * heuristic described on top of this file.
 *
 * PRE: net != NULL
 *	new_err >= 0.0
 *
 * POS: etha state inside net has been updated
 */
static void
anfis_update_etha (anfis_t net, double new_err)
{
	assert (net != NULL);
	assert (new_err >= 0.0);
	
	/** Error descent */
	if (new_err <= net -> old_err1) {
		
		/* If we were in an increasing state... */
		if (net->trend == INCREASE) {
			/* ...for the INC_H consecutive time... */
			if (++(net->trend_st) == INC_H) {
				/* ...increase etha */
				debug ("\n%s\n","η++");
				net->etha += INC_C * net->etha;
				net->trend = FIXED;
				net->trend_st = 0;
			} /* ...else just increment trend_st */
		
		/* If we were in a fixed state... */
		} else if (net->trend == FIXED) {
			/* ... now we are increasing */
			net->trend = INCREASE;
			net->trend_st = 1;
		
		/* If we were in a descending state... */
		} else {
			/* ...and had 1 up, 1 down DEC_H consecutive times... */
			if ( (net->old_err1 > net->old_err2)  &&
				(++(net->trend_st) == DEC_H) )
			{	/* ...decrease etha */
				debug ("\n%s\n","η--");
				net->etha -= DEC_C * net->etha;
				net->trend = FIXED;
				net->trend_st = 0;
			/* ... else if up-down chain was broken... */
			} else if (net->old_err1 <= net->old_err2) {
				/* ...then tables have turned */
				net->trend = INCREASE;
				net->trend_st = 2;
			} /* ...else just increment trend_st */
		}
	
	/** Error augmnet */
	} else {
		/* If we were increasing or fixed ... */
		if (net->trend == INCREASE || net->trend == FIXED) {
			/* ...everything went to hell */
			net->trend = DECREASE;
			net->trend_st = 0;
		
		/* If we were decreasing... */
		} else {
			/* ...and beforehand the error had descended... */
			if (net->old_err1 <= net->old_err2) {
				/* ...we are one step closer to decrease etha */
				net->trend_st++;
			
			/* ...else up-down chain was broken... */
			} else {
				/* ...and we switch to a fixed state */
				net->trend = FIXED;
				net->trend_st = 0;
			}
		}
	}
	
	/* And finally we reflect the time change */
	net->old_err2 = net->old_err1;
	net->old_err1 = new_err;
	
	return;
}



/* Applies the updates stored in 'db_e' to all membership functions inside 'net'
 * 'new_err' must be the total learning error, ie: sum all over db_e values
 *
 * PRE: net  != NULL
 *	db_e != NULL
 *	new_err >= 0.0
 *	
 * POS: network parameters successfully updated
 */
static void
anfis_do_mf_up (anfis_t net, gsl_matrix *db_e, double new_err)
{
	int i = 0, l = 0;
	double	etha = 0.0,
		*db_e_ij = NULL;
	
	assert (net  != NULL);
	assert (db_e != NULL);
	assert (new_err >= 0.0);
	
	etha = net->etha / ((new_err !=0) ? new_err : 2.0);
	
	#pragma omp parallel for default(shared) private(i,l,db_e_ij)
	for (i=0 ; i < net->t * net->n ; i++) {
		
		/* Error gradients for the parameters of M[i/n][i%n] */
		db_e_ij = gsl_matrix_ptr (db_e, i, 0);
		
		for (l=0 ; l < MAX_PARAM ; l++) {
			net-> b[i/net->n]. MF[i%net->n]. p[l] -= etha * db_e_ij[l];
		}
	}
	
	return;
}





/* Performs the gradient descent technique to update premise parameters in net
 *
 * PARAMETERS:  A ------> predictor variables matrix used for LSE
 *		s ------> training samples set
 *		MF -----> matrix containing the network's membership values
 *			  for the last training sample processed
 *		b_tau --> barred taus for the last training sample processed
 *		cp -----> network's consequent parameters in ascending order
 *			  That is: p[0][0], p[0][1], ..., p[t][n]
 *		P ------> # of samples in the last training sample processed
 *
 * PRE: net   != NULL
 *	A     != NULL
 *	MF    != NULL
 *	b_tau != NULL
 *	cp    != NULL
 *
 * POS: result == ANFIS_OK   &&   premise parameters inside net were updated
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_grad_desc (anfis_t net, const t_sample *s, const gsl_matrix *A,
		 gsl_matrix *MF, gsl_matrix *b_tau, gsl_vector *cp, unsigned int P)
{
	gsl_vector *f_out = NULL;
	gsl_matrix *db_e  = NULL;
	double	err_grad  = 0.0,
		total_err = 0.0;
	int i = 0, j = 0;
	int res = ANFIS_OK;
	mfu_t mfu;
	
	assert (net   != NULL);
	assert (A     != NULL);
	assert (MF    != NULL);
	assert (b_tau != NULL);
	assert (cp    != NULL);
	
	f_out = gsl_vector_calloc (P);
	handle_error_2 (f_out);
	
	/* Computing network outputs for the last training sample
	 * using the predictor variable matrix, since: net_outputs == A*cp
	 * This routine performs the computation: f_out = 1.0*A*cp + 0.0*f_out
	 */
	gsl_blas_dgemv (CblasNoTrans, 1.0, A, cp, 0.0, f_out);
	
/*	debug ("%s\n","Network output for this sample:");
	dfor (i=0 ; i<P ; i++) {
		debug ("f_out[%d] = %f\n", i, gsl_vector_get (f_out, i));
	}
	debug ("%s","\n");
*/	
	/* The k-th row of "db_e" will hold the error gradient corresponding
	 * to the parameters update of the network membership function
	 * located in branch # k/n, for input element k%n (ie: MF[k/n][k%n])
	 */
	db_e = gsl_matrix_alloc (net->t * net->n, MAX_PARAM);
	handle_error_2 (db_e);
	
	mfu.s      = s;
	mfu.f_out  = f_out;
	total_err  = 0.0;
	for (i=0 ; i < net->t && res == ANFIS_OK; i++) {
		gsl_vector_view p_sub, b_tau_i, MF_val;
		
		/* p_sub holds paramaters p[i][0], p[i][1], ..., p[i][n] */
		p_sub = gsl_vector_subvector (cp, i*(net->n+1), net->n+1);
		/* b_tau_i holds b_tau values of i-th branch (all P inputs) */
		b_tau_i = gsl_matrix_column (b_tau, i);
		
		#pragma omp parallel for default(shared) \
					 private(j, err_grad, MF_val) \
					 firstprivate(mfu) \
					 reduction(+:total_err) \
					 if(res==ANFIS_OK)
		for (j=0 ; j < net->n ; j++) {
			gsl_vector_view db_e_ij;
			
			if (res != ANFIS_OK)
				continue;
			
			/* MF_val holds MF[i][j] values for all P inputs */
			MF_val = gsl_matrix_column (MF, i * net->n + j);
			/* Error gradients for the parameters of M[i][j] */
			db_e_ij = gsl_matrix_row (db_e, i * net->n + j);
			
			mfu.mf_ij   = &(net->b[i].MF[j]);
			mfu.p_sub   = &(p_sub.vector);
			mfu.b_tau   = &(b_tau_i.vector);
			mfu.MF_val  = &(MF_val.vector);
			mfu.db_e_ij = &(db_e_ij.vector);
			
			/* Computing update values */
			err_grad = anfis_calc_mf_up (&mfu, j, P, net->n);
			
			if (isnan(err_grad)) {
				/* An error ocurred */
				#pragma omp atomic
				res += ANFIS_ERR;
			} else {
				total_err += err_grad * err_grad;
			}
		}
	}
	
	if (res == ANFIS_OK) {
		/* NOTE: Heuristic for etha modification: see top of file */
		anfis_update_etha (net, sqrt (total_err));
		/* Performing parameters update for the whole network */
		anfis_do_mf_up (net, db_e, sqrt (total_err));
		
	} else {
		fprintf (stderr, "anfis.c: anfis_grad_desc: Error updating "
				 "parameters  ~  ABORTING UPDATE !!!\n\n");
	}
	
	/* Cleanup */
	gsl_vector_free (f_out);
	f_out = NULL;
	gsl_matrix_free (db_e);
	db_e = NULL;
	
	return res;
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
 *	P > (network # of branches) * (network input dimension + 1)
 *
 * POS:	result == ANFIS_OK   &&   net's parameters have been updated
 *				  sample was not modified
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_train (anfis_t net, t_sample *s, unsigned int P)
{
	int res = ANFIS_OK;
	long k = 0, i = 0;
	size_t n = 0, t = 0, M = 0;
	gsl_matrix  *MF    = NULL,	/* Membership values for input */
		    *b_tau = NULL,	/* Barred taus for input */
		    *A     = NULL;	/* Predictor variables for LSE */
	gsl_vector  *ccp   = NULL;	/* Copy of new consequent parameters */
	double value = 0.0;
	
	assert (net != NULL);
	assert (s   != NULL);
	
	t = (size_t) net->t;
	n = (size_t) net->n;
	M = t * (n+1);
	
	assert (P > M);
	
	/* Membership values for all the P inputs */
	MF = gsl_matrix_alloc (P, t*n);
	handle_error_2 (MF);
	
	/* Barred taus for all the P inputs */
	b_tau = gsl_matrix_alloc (P, t);
	handle_error_2 (b_tau);
	
	/* Predictor variables matrix for LSE. See below 'A' layout */
	A = gsl_matrix_alloc (P, M);
	handle_error_2 (A);
	
	
	/* Performing P partial propagations to compute A matrix */
	for (k=0 ; k < P ; k++) {
		gsl_vector_view  MF_k    = gsl_matrix_row (MF,    k),
				 b_tau_k = gsl_matrix_row (b_tau, k);
		
		/* Filling MF matrix k-th row */
		res = anfis_compute_membership (net, s[k].in, &(MF_k.vector));
		handle_error_1 (res);
		
		/* Filling b_tau matrix k-th row */
		res = anfis_partial_fwd_prop (net, s[k].in, &(MF_k.vector),
					      &(b_tau_k.vector));
		handle_error_1 (res);
		
		/* Filling A matrix k-th row (see below) */
		#pragma omp parallel for default(shared) private(i,value)
		for (i=0 ; i < M ; i++) {
			
			value = gsl_matrix_get (b_tau, k, i/(n+1));
			if (i % (n+1)) {
				value *= gsl_vector_get (s[k].in, (i%(n+1))-1);
			}
			
			gsl_matrix_set (A, k, i, value);
		}
	}
	
	/* Computing best consequent parameters with LSE */
	ccp = anfis_lse (net, A, s, P);
	if (ccp == NULL) {
		res = ANFIS_ERR;
	} else {
		/* Updating premise parameters with gradient descent method */
		res = anfis_grad_desc (net, s, A, MF, b_tau, ccp, P);
		handle_error_1 (res);
	}
	
	gsl_matrix_free (A);		A     = NULL;
	gsl_matrix_free (MF);		MF    = NULL;
	gsl_matrix_free (b_tau);	b_tau = NULL;
	gsl_vector_free (ccp);		ccp   = NULL;

	return res;
}
/* Matrix 'A' has the following layout:
_________________________________________________________________________________
| b_tau[0] | b_tau[0]*x[0,0]   | b_tau[0]*x[0,1]   | ... | b_tau[t-1]*x[0,n-1]  |
| b_tau[0] | b_tau[0]*x[1,0]   | b_tau[0]*x[1,1]   | ... | b_tau[t-1]*x[1,n-1]  |
|   ...    |       ...         |       ...         | ... |          ...         |
| b_tau[0] | b_tau[0]*x[P-1,0] | b_tau[0]*x[P-1,1] | ... | b_tau[t-1]*x[P-1,n-1]|
=================================================================================
*/



/* Feeds the network with the given input. Returns the network produced output
 *
 * PRE: net   != NULL
 *	input != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == network output for the given input
 */
double
anfis_eval (anfis_t net, gsl_vector *input)
{
	gsl_vector  *MF    = NULL,	/* Membership values for input */
		    *b_tau = NULL;	/* Barred taus for input */
	double out = 0.0, partial = 0.0;
	int i = 0, res = 0;
	
	assert (net   != NULL);
	assert (input != NULL);
	
	/* Generating workspace */
	MF = gsl_vector_alloc (net->t * net->n);
	if (MF == NULL) {
		perror ("anfis.c: anfis_eval: Not enough memory to perform "
			"computation   ~   ABORTING PROPAGATION\n");
		goto exit_point;
	}
	b_tau = gsl_vector_alloc (net->t);
	if (MF == NULL) {
		perror ("anfis.c: anfis_eval: Not enough memory to perform "
			"computation   ~   ABORTING PROPAGATION\n");
		goto exit_point;;
	}
	
	
	/* Filling MF vector */
	res = anfis_compute_membership (net, input, MF);
	if (res != ANFIS_OK) {
		fprintf (stderr, "anfis.c: anfis_eval: Unable to compute input "
				 "membership values   ~   ABORTING PROPAGATION\n");
		goto exit_point;
	}
		
	/* Filling b_tau vector */
	res = anfis_partial_fwd_prop (net, input, MF, b_tau);
	if (res != ANFIS_OK) {
		fprintf (stderr, "anfis.c: anfis_eval: Unable to perform "
				 "forward propagation   ~   ABORTING PROPAGATION\n");
		goto exit_point;
	}
	
	out = 0.0;
	for (i=0 ; i < net->t ; i++) {
		gsl_vector_view p_i;
		
		/* p_i = (p[i][1] , p[i][2] , ... , p[i][n]) */
		p_i = gsl_vector_subvector (net->b[i].P, 1, net->n);;
		
		partial = 0.0;
		/* partial = p_i*input = p[i][1]*x[1] + ... + p[i][n]*x[n] */
		gsl_blas_ddot (&(p_i.vector), input, &partial);
		/* partial = partial + p[i][0] */
		partial += gsl_vector_get (net->b[i].P, 0);
		
		out += partial * gsl_vector_get (b_tau, i);
	}
	
	exit_point:
	{
		if (MF != NULL) {
			gsl_vector_free (MF);
			MF    = NULL;
		}
		if (b_tau != NULL) {
			gsl_vector_free (b_tau);
			b_tau = NULL;
		}
		
		return out;
	}
}

