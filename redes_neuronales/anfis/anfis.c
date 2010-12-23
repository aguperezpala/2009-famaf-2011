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
#include <gsl/gsl_cblas.h>

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
  #define  handle_error_3(arr)   if ((arr) == NULL) return NULL;
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
#else
  #define  handle_error_1(err)
  #define  handle_error_2(arr)
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
};

#define  INIT_ETHA  0.1


/* Internal TAD for computing the premises parameters update
 * (ie: MF internal parameters) using the gradient descent method
 */
typedef struct {
	MT_t *mf_ij;	    /* MF whose parameters must be updated */
	t_sample   *s;      /* samples training set for this epoch */
	gsl_vector *MF_val; /* MF values for all P inputs in the given sample */
	gsl_vector *b_tau;  /* barred tau values (in MF's branch) of sample */
	gsl_vector *p_sub;  /* consequent parameters (in MF's branch) */
	gsl_vector *f_v;    /* network output values for the given sample */
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
	net->etha = INIT_ETHA;
	
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
		if (tau_sum != 0.0) {
			gsl_matrix_set (b_tau, k, i, tau[i] / tau_sum);
		} else {
			gsl_matrix_set (b_tau, k, i, 0.0);
		}
	}
	
	tau   = NULL;
	
	return ANFIS_OK;
}




/* Performs least square estimate (aka LSE) for the given sample (of size P)
 * to find best values for the network's consequent parameters
 *
 * The network is updated internally, and a copy of this new values is returned
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
	gsl_multifit_linear_workspace *work = NULL;
	gsl_matrix *cov = NULL;	/* Covariance matrix generated for LSE */
	gsl_vector *y = NULL,	/* Sample results (P in total) */
		   *p = NULL;	/* Estimated parameters (M in total) */
	size_t M = 0;
	unsigned int i = 0, j = 0;
	double err = 0.0;
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
	
	/* Generating necessary workspace */
	p = gsl_vector_calloc (M);
	handle_error_3 (p);
	cov = gsl_matrix_calloc (M, M);
	handle_error_3 (cov);
	work = gsl_multifit_linear_alloc ((size_t) P, M);
	handle_error_3 (work);
	
	/* Performing LSE */
	gsl_multifit_linear (A, y, p, cov, &err, work);
	err = err;
	
	/* Hastly freeing the insulting amount of memory reserved */
	gsl_multifit_linear_free (work);
	work = NULL;
	gsl_matrix_free (cov);
	cov = NULL;
	gsl_vector_free (y);
	y = NULL;
	
	/* Saving estimated parameters inside the network */
	debug ("LSE parameters estimation (call # %lu):\n", ++calls);
	for (i=0 ; i < net->t ; i++) {
		/* p_sub references the i-th branch consequent parameters
		 * estimation, originally stored in vector p */
		gsl_vector_view p_sub;
		
		p_sub = gsl_vector_subvector (p, i*(net->n+1), net->n+1);
		gsl_vector_memcpy (net->b[i].P, &p_sub.vector);
		
		dfor (j=0 ; j <= net->n ; j++) {
			debug ("p[%u][%u] = %.4f\n", i, j,
			       gsl_vector_get (p, i*(net->n+1) + j));
		}
	}
	
	return p;
}




/* Computes error gradient for all parameters of the given membership function.
 * Each parameter's err.grad. is stored in vector db_e.
 * The whole learning error is the returned value.
 *
 * PARAMETERS:  mfu ---> MF update TAD
 *		db_e --> error gradient in this epoch (for each parameter)
 *		P -----> # of samples in the set
 *		j -----> order # of the input element this MF processes
 *		n -----> network input dimension
 *
 * PARAMETERS INSIDE mfu:
 *		mf_ij ---> membership function whose parameters must be updated
 *		s -------> samples training set
 *		MF_val --> MF values for all P inputs in the given sample
 *		b_tau ---> barred tau values for all P inputs (for this branch)
 *		p_sub ---> consequent parameters (for this branch)
 *		f_v -----> network output values for the given sample
 *		
 * PRE: mfu != NULL
 *	All values inside mfu != NULL
 *	All vectors dimension == P
 *	j < n
 *
 * USE: err = anfis_update_mf (&mfu, db_e, P, j, n)
 *
 * POS: err >= 0   &&   gradients stored in db_e   &&   learning error returned
 *	or
 *	err <  0   &&   an internal misfunction has ocurred
 */
static double
anfis_calc_mf_up (mfu_t *mfu, double db_e[MAX_PARAM],
		  unsigned int P, unsigned int j, unsigned int n)
{
	unsigned int k = 0;
	MF_t *mf_ij = NULL;	/* MF whose parameters must be updated */
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
	assert (mfu->f_v    != NULL);
	assert (j < n);
	
	mf_ij = mfu -> mf_ij;
	
	/** TODO: erase the following check and implement support for
	 **	  every membership function defined in the header file
	 **/
	if (mf_ij -> k != bell) {
		fprintf (stderr, "Membership function not implemented\n");
		return -1.0;
	}
	
	/* Initializing errors vector */
	for (int l = 0 ; l < MAX_PARAM ; l++) {
		db_e[l] = 0.0;
	}
	
	
	/** WARNING Following loop works only for MF of kind 'bell' */
	#pragma omp parallel for default(shared) \
		private(k, mf_k, w, xj, fk, p_aux, pk, tk, yk, aux)
	for (k=0 ; k < P ; k++) {
		gsl_vector_view p_aux;
		
		/* mf_k = MF value for k-th sample */
		mf_k = gsl_vector_get (mfu -> MF_val, k);
		w    = mf_k * (1.0 - mf_k);
		
		/* xj = j-th input element */
		xj = gsl_vector_get ((mfu -> s)[k].in, j);
		
		/* fk = network output for k-th sample */
		fk = gsl_vector_get (mfu -> f_v, k);
		
		/* pk = p[i][0] + p[i][1] * x[k][1] + ... + p[i][n-1] * x[k][n-1] */
		p_aux = gsl_vector_subvector (mfu -> p_sub, 1, n);
		gsl_blas_ddot (&p_aux.vector, (mfu -> s)[k].in, &pk);
		pk += gsl_vector_get (mfu -> p_sub, 0);
		
		/* tk = barred tau for k-th sample (of this branch) */
		tk = gsl_vector_get (mfu -> b_tau, k);
		
		/* yk = correct output for k-th sample */
		yk = (mfu -> s)[k].out;
		
		#pragma omp sections
		{
			/* Update of first param ('a') */
			#pragma omp section
			{
				d_mf[0] = (-2.0 * w * mf_ij->p[1]) / mf_ij->p[0];
				db_e[0] += -2.0 * (yk - fk) * (pk - fk)
						* tk * (1.0/mf_k) * d_mf[0];
			}
			
			/* Update of second param ('b') */
			#pragma omp section
			{
				if (xj != mf_ij->p[2]) {
					aux = (xj - mf_ij->p[2]) / mf_ij->p[a];
					d_mf[1]  = -2.0 * w * log (fabs (aux));
					db_e[1] += -2.0 * (yk - fk) * (pk - fk)
							* tk * (1.0/mf_k) * d_mf[1];
				}
			}
			
			/* Update of third param ('c') */
			#pragma omp section
			{
				if (xj != mf_ij->p[2]) {
					d_mf[2] = (-2.0 * w * mf_ij->p[1]) /
						  (xj - mf_ij->p[2]);
					db_e[2] += -2.0 * (yk - fk) * (pk - fk)
							* tk * (1.0/mf_k) * d_mf[2];
				}
			}
		}
	}
	
	aux = 0.0;
	for (int l = 0 ; l < MAX_PARAM ; l++) {
		aux += db_e[l];
	}
	
	return aux;
}




/* Applies the update to the parameters of the given MF
 * Etha is updated according to this epoch learning error
 *
 * PRE: mf_ij != NULL
 *	etha  != NULL
 *
 * POS: result == ANFIS_OK   &&   (parameters + etha) successfully updated
 *	or
 *	result == ANFIS_ERR
 */
static int
anfis_do_mf_up (const double db_e[MAX_PARAM], MF_t *mf_ij, double *etha)
{
	assert (mf_ij != NULL);
	assert (etha  != NULL);
	
	/** TODO: fill'it up */
	
	return ANFIS_OK;
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
anfis_grad_desc (anfis_t net, t_sample *s, const gsl_matrix *A, const gsl_matrix *MF,
		  const gsl_matrix *b_tau, const gsl_vector *cp, unsigned int P)
{
	gsl_vector f_v = NULL;
	double	_a  = 1.0,
		_b  = 1.0,
		err = 0.0,
		db_e[MAX_PARAM];
	int i = 0, j = 0;
	mfu_t mfu;
	
	assert (net   != NULL);
	assert (A     != NULL);
	assert (MF    != NULL);
	assert (b_tau != NULL);
	assert (cp    != NULL);
	
	f_v = gsl_vector_calloc (P);
	handle_error_2 (f);
	
	/* Computing network outputs for the last training sample,
	 * with the predictor variable matrix, since: outputs == A cp
	 * This routine performs the computation: f_v = _a*A*cp + _b*f_v
	 */
	gsl_blas_dgemv (CblasNoTrans, _a, A, cp, _b, f_v);
	
	mfu.s   = s;
	mfu.f_v = f_v;
	
	for (i=0 ; i < net->t ; i++) {
		gsl_vector_view p_sub, b_tau_i, MF_val;
		
		/* p_sub holds paramaters p[i][0], p[i][1], ..., p[i][n] */
		p_sub = gsl_vector_subvector (p, i*(net->n+1), net->n+1);
		/* b_tau_i holds b_tau values of i-th branch (all P inputs) */
		b_tau_i = gsl_matrix_column (b_tau, i);
		
		for (j=0 ; j < net->n ; j++) {
			
			/* MF_val holds MF[i][j] values for all P inputs */
			MF_val = gsl_matrix_column (MF, i*n+j);
			
			mfu.mf_ij  = &(net->b[i].MF[j]);
			mfu.p_sub  = &p_sub.vector;
			mfu.b_tau  = &b_tau_i.vector;
			mfu.MF_val = &MF_val.vector;
			
			err = anfis_calc_mf_up (mfu, P, j, net->n, db_e);
			if (err < 0.0) {
				return ANFIS_ERR;
			} else {
				anfis_do_mf_up (db_e, mfu.mf_ij, &(net->etha));
			}
		}
	}
	
	gsl_vector_free (f_v);
	f_v = NULL;
	
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
 *	P > (network # of branches) * (network input dimension + 1)
 *
 * POS:	result == ANFIS_OK   &&   net's parameters have been updated
 *	or
 *	result == ANFIS_ERR
 */
int
anfis_train (anfis_t net, const t_sample *s, unsigned int P)
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
	
	/* Computing best consequent parameters with LSE */
	ccp = anfis_lse (net, A, s, P);
	handle_error_2 (ccp);
	
	/* Updating premise parameters with gradient descent method */
	res = anfis_grad_desc (net, s, A, MF, b_tau, ccp, P);
	handle_error_1 (res);
	
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

