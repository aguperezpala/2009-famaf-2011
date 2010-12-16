/*
 * Neural Networks - Three-layers perceptron (ie: one hidden layer)
 * Network ADT
 *
 * WARNING: everything here is awfully hardcoded for efficiency's sake
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 13/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include <inttypes.h>
#include <limits.h>
#include <float.h>

#include "ptron_3l.h"
#include "mzran13.h"

struct _ptron3_s {
	int N[NLAYERS];		/* # of neurons per layer*/
	double (*f) (double);	/* Sinaptic function */
	double (*df) (double);	/* f's derivative */
	
	double *V [NLAYERS];	/* Neurons. Input layer (V[0]) is NULL */
	double *w [NLAYERS-1];	/* Sinaptic weights */
	
	double *d [NLAYERS];	/* Auxiliary values for dw calculation */
	double *dw[NLAYERS-1];	/* Back-propagation updates */
	double *old_dw[NLAYERS-1];	/* Updates of previous step	*
					 * (for momentum update mode)	*/
	double *err;		/* Learning errors (Ei) */
	double error;		/* Last error (Ei sumation) */
	
	double etha;		/* updates scaling parameter */
	double alpha;		/* previous update importance (mom update) */
	short rdy;		/* is the network ready for fwd-prop? */
	short fwd;		/* has a fwd propagation recently been done? */
};


#define  INPUT  0

#define  MAX(x,y)	((x) > (y) ? (x) : (y))
#define  MIN(x,y)	((x) < (y) ? (x) : (y))

#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long))
#define  int_val(x)	((x) & (~(((unsigned long) 1) << (MSB-1))))


/* Debugging printing functions */
#ifdef _DEBUG
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
  static void
  printbits (uint64_t n)
  {
	uint64_t l = ((uint64_t)1) << 63;
	short k = 1;
	
	while (l > 0) {
		debug ("%c", n&l ? '1' : '0');
		l >>= 1;
		!(k++ % _byte_size) ? debug("%s"," ") : 0;
	}
	debug ("%s","\n");
  }
#else
  #define  debug(s,...)
  #define  dfor(s)
  #define  printbits(n)
#endif




/** ### ### ### ~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Creates an instance of the ADT
 *
 * PARAMETERS:	N[i] --> # of neurons of the i-th layer.
 *			 N ranges from 0 (input layer) to NLAYERS (output layer)
 *		etha --> initial value for the proportionality constant
 *			 of the gradient descent learning heuristic
 *		alpha -> importance of the previous update value,
 *			 for the "momentum" update mode
 *		f -----> sinaptic update function
 *		df ----> function f derivative
 *
 * POS: result != NULL
 *
 * NOTE: In the current implementation function 'f' is ignored
 *	 Predefined function g(x) is used instead
 *	 Same for 'df'
 */
ptron3_t
ptron_create (const unsigned int N[NLAYERS], double etha, double alpha,
	      double (*f) (double), double (*df) (double))
{
	ptron3_t net = NULL;
	int m = 0;
	
	net = (ptron3_t) malloc (sizeof (struct _ptron3_s));
	assert (net != NULL);
	
	/* Size of each layer */
	for (m=0 ; m < NLAYERS ; m++) {
		net->N[m] = N[m];
	}
	
	/* Network neurons
	 * We create N[m]+1 places in order to store the threshold value
	 * First layer (V[0], aka "input layer") is left NULL */
	for (m=0 ; m < NLAYERS ; m++) {
		net->V[m] = (double *) malloc ((N[m]+1) * sizeof(double));
		assert (net->V[m] != NULL);
		/* Last place of all layers is -1
		 * to simulate threshold inhibition */
		net->V[m][N[m]] = -1.0;
	}
	
	/* Sinaptic weights and related paraphernalia
	 * Again N[m]+1 is used to include threshold value */
	for (m=0 ; m < NLAYERS-1 ; m++) {
		
		net->w[m] = (double *) calloc ((N[m]+1) * N[m+1] , sizeof (double));
		assert (net->w[m] != NULL);
		
		net->dw[m] = (double *) calloc ((N[m]+1) * N[m+1] , sizeof (double));
		assert (net->dw[m] != NULL);
		
		net->d[m+1] = (double *) calloc ((N[m+1]+1) , sizeof (double));
		assert (net->d[m+1] != NULL);
		/* Notice d[0] is a NULL vector */
		
		net->old_dw[m] = (double *) calloc ((N[m]+1) * N[m+1] ,
				  			sizeof (double));
		assert (net->old_dw[m] != NULL);
	}
	net->d[0] = NULL;
	
	net->err = (double *) malloc (N[NLAYERS-1] * sizeof (double));
	assert (net->err != NULL);
	
	net->f     = f;
	net->df    = df;
	net->etha  = etha;
	net->alpha = alpha;
	net->error = 0.0;
	net->rdy   = 0;
	net->fwd   = 0;
	
	return net;
}




/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net);
 */
ptron3_t
ptron_destroy (ptron3_t net)
{
	int m = 0;
	
	assert (net != NULL);
	
	for (m=0 ; m < NLAYERS-1 ; m++) {
		free (net->V[m]);	net->V[m]   = NULL;
		free (net->w[m]);	net->w[m]   = NULL;
		free (net->dw[m]);	net->dw[m]  = NULL;
		free (net->d[m+1]);	net->d[m+1] = NULL;
		free (net->old_dw[m]);	net->old_dw[m] = NULL;
	}
	free (net->V[m]);	net->V[m] = NULL;
	free (net->err);	net->err  = NULL;
	
	free (net);
	net = NULL;
	
	return net;
	printbits(0);
}




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


/* Restarts the sinaptic weights to random values between bounds given.
 * This also changes the threshold for the input and middle layers.
 *
 * PRE: net != NULL
 *	upBound > lowBound
 *
 * POS: result == PTRON_OK  &&  sinaptic weights reinitialized
 *	or
 *	result == PTRON_ERR
 */
int
ptron_reinit_weights (ptron3_t net, double lowBound, double upBound)
{
	int res = PTRON_OK;
	int m = 0, i = 0;
	double ran = 0.0;
	
	assert (net != NULL);
	assert (upBound > lowBound);
	
	net->rdy = 1;
	
	debug("%s\n","Restarting sinaptic weights");
	/* WARNING: do not parallelize this loop
	 *	    mzran13 is NOT threadsafe
	 */
	for (m=0 ; m < NLAYERS-1 ; m++) {
		
		debug ("%s\n","----------------------------------");
		for (i=0 ; i < ((net->N[m]+1) * net->N[m+1]) ; i++) {
			
			ran = ((double) mzran13()) / ((double) ULONG_MAX);
			net->w[m][i] = ran * (upBound - lowBound) + lowBound;
#ifdef _DEBUG
			debug("w[%d][%d,%d] = %.4f\n", m,
			      i%(net->N[m]+1), i/(net->N[m]+1), net->w[m][i]);
			if (net->w[m][i] < lowBound || net->w[m][i] > upBound) {
				res = PTRON_ERR;
				net->rdy = 0;
			}
#endif
		}
		debug("%s","\n");
	}
	
	return res;
}




/* Stores in N the # of neurons of each layer in th network
 * including both input and output layers.
 *
 * PRE: net != NULL
 *
 * POS: sizes stored in N
 */
void
ptron_get_layers_size (ptron3_t net, unsigned int N[NLAYERS])
{
	int m = 0;
	
	assert (net != NULL);
	
	for (m=0 ; m < NLAYERS ; m++) {
		N[m] = net->N[m];
	}
	
	return;
}




/* Gets the network output layer values
 * Caller owns the memory allocated for returned vector
 * Free using glibc standard free() routine
 *
 * PRE: net != NULL
 *
 * USE: Out = ptron_get_output (net)
 *
 * POS: Out != NULL  &&  result stored in vector 'Out'
 *	or
 *	Out == NULL
 */
double *
ptron_get_output (ptron3_t net)
{
	int i = 0;
	double *O = NULL;
	
	assert (net != NULL);
	
	O = (double *) malloc (net->N[NLAYERS-1] * sizeof (double));
	if (O != NULL) {
		for (i=0 ; i < net->N[NLAYERS-1] ; i++)
			O[i] = net->V[NLAYERS-1][i];
	}
	
	return O;
}




/* Computes the learning error: the difference between the output produced
 * in the last forward propagation and the expected result given in vector 'NU'
 *
 * NOTE: error calculation is done incrementally, meaning each time
 *	 ptron_get_error() is called on net, the distance between
 *	 the newly produced output and the expected result 'NU'
 *	 IS ADDED UP internally to previous values.
 *	
 *	 To reset error values call ptron_reset(net)
 *
 * PRE: net != NULL
 *	NU  != NULL
 *	length_of(NU) >= length_of(output-layer)
 *
 * POS: result == last propagation's learning error: the error sumation of
 *		  every neuron in the output layer, disregarding prev. results
 *	or
 *	result == DBL_MAX  &&  ptron_fwd_prop() must be invoked beforehand
 *
 */
double
ptron_calc_err (ptron3_t net, double *NU)
{
	double err = DBL_MAX, diff = 0.0;
	int i = 0;
	
	assert (net != NULL);
	assert (NU  != NULL);
	
	err = 0.0;
	for (i=0 ; i < net->N[NLAYERS-1] ; i++) {
		diff = net->V[NLAYERS-1][i] - NU[i];
		net->err[i] += diff * diff;
		err += diff * diff;
	}
	
	return 0.5 * err;
}




/* Returns the accumulated learning error value for each output layer neuron
 * Caller owns the memory allocated for returned vector
 * Free using glibc standard free() routine
 * 
 * PRE: net != NULL
 *
 * USE: Err = ptron_get_output (net)
 *
 * POS: Err != NULL  &&  errors stored in vector 'Err'
 *	or
 *	Err == NULL
 */
double *
ptron_get_err (ptron3_t net)
{
	double *err = NULL;
	int i = 0;
	
	assert (net != NULL);
	
	err = (double *) malloc (net->N[NLAYERS-1] * sizeof (double));
	if (err != NULL) {
		for (i=0 ; i < net->N[NLAYERS-1] ; i++) {
			err[i] = net->err[i];
		}
	}
	
	return err;
}




/* Erases weight updates (aka: delta_w) and learning error values
 * stored in the network, setting them all to 0
 * PRE: net != NULL
 */
void
ptron_reset (ptron3_t net)
{
	int m = 0, i = 0;
	
	assert (net != NULL);
	
	#pragma omp parallel for default(shared) private (m,i)
	for (m=0 ; m < NLAYERS-1 ; m++) {
		for (i=0 ; i < ((net->N[m]+1) * net->N[m+1]) ; i++) {
			net->dw[m][i] = 0.0;
			net->old_dw[m][i] = 0.0;
		}
	}
	#pragma omp parallel for default(shared) private (i)
	for (i=0 ; i < net->N[NLAYERS-1] ; i++) {
		net->err[i] = 0.0;
	}
	
	return;
}





/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Performs forward propagation of the input value "XI" across the network.
 * XI is of length "len", and at most length_of(input-layer) elements of it
 * will be used in the propagation.
 *
 * PRE: net != NULL
 *	weights in net have been initialized at least once
 *	XI != NULL
 *
 * POS: result == PTRON_OK  &&  values successfully propagated
 *	or
 *	result == PTRON_ERR
 */
int
ptron_fwd_prop (ptron3_t net, const double *XI, size_t len)
{
	int res = PTRON_OK;
	int m = 0, i = 0, k = 0;
	size_t size = 0;
	double sum = 0.0;
	
	assert (net != NULL);
	assert (net->rdy);
	assert (XI != NULL);
	
	size = MIN (net->N[INPUT], len) * sizeof (double) ;
	net->V[INPUT] = (double *) memcpy (net->V[INPUT], XI, size);
	
	debug ("%s\n","Received input:");
	dfor (i=0 ; i < net->N[INPUT] ; i++) {
		debug("INPUT[%d] = %.3f\n", i, net->V[INPUT][i]);
		if (net->V[INPUT][i] != XI[i]) {
			res = PTRON_ERR;
		}
	}
	debug ("%s\n","\nAbout to perform forward propagation");
	
	for (m=0 ; m < NLAYERS-1 ; m++) {
		#pragma omp parallel for default(shared) private(i,k,sum)
		for (i=0 ; i < net->N[m+1] ; i++) {
			/* w[m] rows    span over layer m   (index k) *
			 * w[m] columns span over layer m+1 (index i) */
			
			sum = 0.0;
			for (k=0 ; k < net->N[m] ; k++) {
				sum += net->V[m][k] * net->w[m][i + k*net->N[m+1]];
			}
			/* ... plus the threshold inhibition ... */
			k = net->N[m];
			sum += -1.0 * net->w[m][i + k*net->N[m+1]];
			
			net->V[m+1][i] = g(sum);
		}
	}
	
	debug ("%s\n", "Produced output:");
	dfor (i=0 ; i < net->N[NLAYERS-1] ; i++) {
		debug ("O[%d] = %f\n", i, net->V[net->N[NLAYERS-1]][i]);
	}
	
	net->fwd = 1;
	
	return res;
}




/* ~~~~~~~~~~~~  Auxiliary functions for ptron_back_prop  ~~~~~~~~~~~~~~~~~~~~*/

/* Performs 'd' auxiliary values calculations */
static void
ptron_compute_d (ptron3_t net, double *NU)
{
	int m = 0, i = 0, k = 0;
	double sum = 0.0;
	
	m = NLAYERS-1;
	/* Computing the deltas for the output layer	*
	 * There's no threshold here			*/
	#pragma omp parallel for default(shared) private(i)
	for (i=0 ; i < net->N[m] ; i++) {
		/* The following is correct only when g(x) == tanh(B*x) */
		net->d[m][i] = B * (1.0 - (net->V[m][i] * net->V[m][i])) *
				(NU[i] - net->V[m][i]);
	}
	
	/* Recursively computing (backwards) the deltas for each layer	*
	 * Threshold unit in each layer is also considered		*/
	for (m = NLAYERS-2 ; m > 0 ; m--) {
		
		#pragma omp parallel for default(shared) private(i,k,sum)
		for (i=0 ; i < net->N[m]+1 ; i++) {
			
			/* sum(i) = Σ w[m][i,k] * δ[m+1][k] */
			sum = 0.0;
			for (k=0 ; k < net->N[m+1] ; k++) {
				sum += net->w[m][k + i*net->N[m+1]] *
					net->d[m+1][k];
			}
			/* Only for g(x) == tanh(B*x) */
			net->d[m][i] = B * (1.0 - (net->V[m][i] * net->V[m][i]))
					 * sum;
		}
	}
	return;
}


/* Computes net's dw values (weights updates)  */
static void
ptron_compute_dw (ptron3_t net, ptron_dynamic mode)
{
	int m = 0, i = 0;
	size_t size = 0;
	
	if ((mode == mom || mode == full) && net->rdy < 2) {
		/* Momentum dynamics is used, but this is the first update,
		 * and thus there are still no "past updates". Setting to zero.
		 */
		#pragma omp parallel for default(shared) private(m)
		for (m=0 ; m < NLAYERS-1 ; m++) {
			size = (net->N[m]+1) * net->N[m+1] * sizeof (double);
			net->old_dw[m] = memset (net->old_dw[m], 0, size);
		}
		
		net->rdy = 2;
	}
	
	/* Computing parallely all weight updates */
	#pragma omp parallel for default(shared) private(m,i)
	for (m=0 ; m < NLAYERS-1 ; m++) {
		for (i=0 ; i < ((net->N[m]+1) * net->N[m+1]) ; i++) {
			/* Threshold weights are modified as well */
			net->dw[m][i] += net->etha *
					 net->d[m+1][i % net->N[m+1]] *
					 net->V[m]  [i / net->N[m+1]];
			net->dw[m][i] += (mode!=mom && mode!=full) ? 0.0 :
					 net->alpha * net->old_dw[m][i];
		}
	}
	
	/* Actual updates now become the old updates */
	#pragma omp parallel for default(shared) private(m)
	for (m=0 ; m < NLAYERS-1 ; m++) {
		size = (net->N[m]+1) * net->N[m+1] * sizeof(double);
		net->old_dw[m] = memcpy (net->old_dw[m], net->dw[m], size);
	}
	
	return;
}



/* Performs backwards propagation computations after a processed sample.
 * NU holds the expected output for the last forward propagation.
 *
 * This can be done (successfully) only once after each forward propagation.
 * The "mode" parameter is for choosing among the different update alternatives
 * (ie: standard, with weight momentum, with adaptive parameter ETHA ...)
 *
 * NOTE: just as ptron_calc_err(), this funtion performs an internal error
 *	 calculation. So calling ptron_back_prop() and then ptron_calc_err()
 *	 is usually a mistake.
 *
 * NOTE: this function computes the next sinaptic weight updates and stores them
 *	 incrementally, but it DOES NOT PERFORM THE ACTUAL UPDATE
 *	 To do so ptron_do_updates(net) must be invoked
 *	
 *	 "incrementally" means that newly computed updates are added to any
 *	 previous update value stored in the network.
 *
 * PRE: net != NULL
 *	NU  != NULL
 *	length_of(NU) >= length_of(output-layer)
 *
 * POS: result == PTRON_OK  &&  updates successfully computed
 *	or
 *	result == PTRON_ERR  &&  ptron_fwd_prop() must be invoked beforehand
 */
int
ptron_back_prop (ptron3_t net, double *NU, ptron_dynamic mode)
{
	int m = 0, i = 0, k = 0;
	double err = 0.0;
	
	assert (net != NULL);
	assert (NU != NULL);
	
	if (!net->fwd)
		return PTRON_ERR;
	else
		net->fwd = 0;
	
	/* Computing auxiliary values d[m][i] */
	ptron_compute_d (net, NU);
	
	/* Computing the updates for this iteration */
	ptron_compute_dw (net, mode);
	
	/* Computing new learning error */
	err = ptron_calc_err (net, NU);
	
	/* Updating parameters if requested */
	if (mode == adp || mode == full) {
		if (err <= net->error) {
			net->etha += INCREASE;
		} else {
			net->etha -= DECREASE * net->etha;
		}
	}
	
	net->error = err;
	
	debug("\n%s\n","Weight updates generated:");
	dfor (m=0 ; m < NLAYERS-1 ; m++) {
		debug ("\nLayer # %d\n", m);
		dfor (i=0 ; i < net->N[m+1] ; i++) {
			dfor (k=0 ; k < net->N[m]+1 ; k++) {
				debug ("dw[%d][%d,%d] = %f\n", m, k, i,
				       net->dw[m][i + k*net->N[m+1]]);
			}
		}
	}
	
	return PTRON_OK;
}




/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: network sinaptic weights updated
 */
void
ptron_do_updates (ptron3_t net)
{
	int m = 0, i = 0;
	
	assert (net != NULL);
	
	#pragma omp parallel for default(shared) private(m,i)
	for (m=0 ; m < NLAYERS-1 ; m++) {
		for (i=0 ; i < ((net->N[m]+1) * net->N[m+1]) ; i++) {
			net->w[m][i] += net->dw[m][i];
		}
	}
	
	return;
}


