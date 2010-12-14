/*
 * Neural Networks - Multi-layer perceptron
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 09/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <limits.h>

#include "ptron.h"
#include "mzran13.h"

struct _ptron_s {
	int A;			/* # of layers (counting input & output) */
	int *N;			/* # of neurons per layer*/
	double etha;		/* updates scaling parameter */
	
	double **V;		/* neurons */
	double **w;		/* sinaptic weights matrix */
	double (*g) (double);	/* propagation function */
	double **delta;		/* for back-propagation updates */
	double **dw;		/* for back-propagation updates */
	
	short ready;		/* is the network ready for fwd-prop? */
	short fwd;		/* has a fwd propagation recently been done? */
};


#define  INPUT  0

#define  MAX(x,y)	((x) > (y) ? (x) : (y))
#define  MIN(x,y)	((x) > (y) ? (y) : (x))

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
 * PARAMETERS:	A -----> # of layers (counting input & output)
 *		N[i] --> # of neurons of the i-th layer.
 *			 N ranges from 0 (input layer) to A (output layer)
 *		g -----> sinaptic update function
 *
 * PRE: N != NULL
 * POS: result != NULL
 */
ptron_t
ptron_create (unsigned int A, const unsigned int *N, double (*g) (double))
{
	ptron_t net = NULL;
	int i = 0;
	
	net = (ptron_t) malloc (sizeof (struct _ptron_s));
	assert (net != NULL);
	
	/* # of layers */
	net->A = int_val(A);
	
	/* etha parameter for net self-update */
	net->etha = 0.1;
	
	/* size of each layer */
	net->N = (int *) calloc (net->A+1, sizeof (int));
	assert (net->N != NULL);
	for (i=0 ; i <= net->A ; i++) {
		net->N[i] = int_val(N[i]);
		debug("N[%d] = %u\n", i, net->N[i]);
	}
	
	/* neurons */
	net->V = (double **) malloc ((net->A + 1) * sizeof (double *));
	assert (net->V != NULL);
	for (i=0 ; i <= net->A ; i++) {
		net->V[i] = (double *) malloc (net->N[i] * sizeof (double));
		assert (net->V[i] != NULL);
	}
	
	/* Let 'n' be the i-th layer size, that is: n == net->N[i]
	 * same with 'm' and layer "i+1"
	 * Then w[i] is a [n]x[m] matrix holding sinaptic weights between
	 * layers 'i' and 'i+1'
	 */
	net->w = (double **) malloc (net->A * sizeof (double *));
	assert (net->w != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->w[i] = (double *) malloc (net->N[i] * net->N[i+1] *
						sizeof (double));
		assert (net->w[i] != NULL);
	}
	
	/* propagation function */
	net->g = g;
	
	/* axuiliary value for the weights update */
	net->delta = (double **) malloc (net->A * sizeof (double *));
	assert (net->delta != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->delta[i] = (double *) malloc (net->N[i+1] * sizeof (double));
		assert (net->delta[i] != NULL);
	}
	
	/* weights update values
	 * net->dw[i] holds the update for the weights in net->w[i]
	 * For that reason net->dw[i] is an [n]x[m] matrix, where
	 * n == net->N[i] and m == net->N[i+1]
	 */
	net->dw = (double **) malloc (net->A * sizeof (double *));
	assert (net->dw != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->dw[i] = (double *) malloc (net->N[i] * net->N[i+1] *
						sizeof (double));
		assert (net->dw[i] != NULL);
	}
	
	net->ready = 0;
	net->fwd = 0;
	
	return net;
}




/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net)
 */
ptron_t
ptron_destroy (ptron_t net)
{
	int i = 0;
	
	assert (net != NULL);
	
	free (net->N);	net->N = NULL;
	for (i=0 ; i < net->A ; i++) {
		free (net->V[i]);	net->V[i] = NULL;
		free (net->w[i]);	net->w[i] = NULL;
		free (net->delta[i]);	net->delta[i] = NULL;
		free (net->dw[i]);	net->dw[i] = NULL;
	}
	free (net->V[net->A]);	net->V[net->A] = NULL;
	
	free (net->V);		net->V = NULL;
	free (net->w);		net->w = NULL;
	free (net->delta);	net->delta = NULL;
	free (net->dw);		net->dw = NULL;
	
	free(net);	net = NULL;
	
	return net;
}




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


/* Restarts the sinaptic weights to random values between bounds given
 *
 * PRE: net != NULL
 *	upBound > lowBound
 *
 * POS: result == PTRON_OK  &&  sinaptic weights reinitialized
 *	or
 *	result == PTRON_ERR
 */
int
ptron_reinit (ptron_t net, double lowBound, double upBound)
{
	int res = PTRON_OK;
	int i = 0, j = 0;
	double ran = 0.0;
	
	assert (net != NULL);
	assert (upBound > lowBound);
	
	net->ready = 1;
	
	debug("%s","Restarting sinaptic weights\n");
	for (i=0 ; i < net->A ; i++) {
		
		for (j=0 ; j < (net->N[i] * net->N[i+1]) ; j++) {
			
			ran = ((double) mzran13()) / ((double) ULONG_MAX);
			net->w[i][j] = ran * (upBound - lowBound) + lowBound;
#ifdef _DEBUG
			debug("w[%d][%d] = %.4f\n", i, j, net->w[i][j]);
			if (net->w[i][j] < lowBound || net->w[i][j] > upBound) {
				res = PTRON_ERR;
				net->ready = 0;
			}
#endif
		}
	}
	
	return res;
}




/* Returns the number of layers the network was created with,
 * including both input and output layer.
 *
 * PRE: net != NULL
 */
unsigned int
ptron_get_num_layers (ptron_t net) { assert (net != NULL); return net->A; }



/* Stores in N the # of neurons of each layer in th network,
 * including both input and output layers.
 * Caller owns the memory stored for array N.
 *
 * PRE: net != NULL
 *	N == NULL
 *
 * USE: N = ptron_get_layers_size (net, N);
 *
 * POS: result != NULL  &&  sizes stored in N
 *	or
 *	result == NULL
 */
unsigned int *
ptron_get_layers_size (ptron_t net, unsigned int *N)
{
	assert (net != NULL);
	assert (N == NULL);
	
	N = (unsigned int *) malloc ((net->A + 1) * sizeof (unsigned int));
	if (N != NULL) {
		memcpy (N, net->N, (net->A + 1) * sizeof (unsigned int));
	}
	
	return N;
}




/* Sets vector XI, of lenght "length", as the network's new input pattern
 * At most length_of(input-layer) elements will be copied out.
 *
 * PRE: net != NULL
 *	XI  != NULL
 *
 * POS: result == PTRON_OK  &&  net's input layer == XI
 *	or
 *	result == PTRON_ERR
 */
int
ptron_set_input (ptron_t net, const double *XI, size_t length)
{
	int i = 0, res = PTRON_OK;
	size_t size = 0;
	
	assert (net != NULL);
	assert (XI != NULL);
	
	size = MIN (net->N[INPUT], length ) * sizeof (double) ;
	
	net->V[INPUT] = (double *) memcpy (net->V[INPUT], XI, size);
	
	debug ("%s","Receiving new input:\n");
	dfor (i=0 ; i < net->N[INPUT] ; i++) {
		debug("INPUT[%d] = %.3f\n", i, net->V[INPUT][i]);
		if (net->V[INPUT][i] != XI[i]) {
			res = PTRON_ERR;
		}
	}
	
	return res;
}




/* Gets the network output layer values
 * Caller owns the memory allocated for vector 'O'
 * Free using glibc standard free() routine
 *
 * PRE: net != NULL
 *	O == NULL
 *
 * USE: O = ptron_get_output (net, O)
 *
 * POS: O != NULL  &&  result stored in vector O
 *	or
 *	O == NULL
 */
double *
ptron_get_output (ptron_t net, double *O)
{
	size_t size = 0;
	
	assert (net != NULL);
	assert (O == NULL);
	
	size = net->N[net->A] * sizeof (double);
	O = (double *) malloc (size);
	
	if (O != NULL) {
		O = memcpy (O, net->V[net->A], size);
	}
	
	return O;
}





/* Erases weight updates calculations (aka: delta_w) stored in the network,
 * setting all of them to zero.
 * PRE: net != NULL
 */
void
ptron_clear_updates (ptron_t net)
{
	int m = 0, i = 0, j = 0;
	
	assert (net != NULL);
	
	#pragma omp parallel for default(shared) private(m,j,i)
	for (m=0 ; m < net->A ; m++) {
		for (i=0 ; i < net->N[m] ; i++) {
			for (j=0 ; j < net->N[m+1] ; j++) {
				net->dw[m][j + i * net->N[m+1]] = 0.0;
			}
		}
	}
	
	return;
}





/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Performs forward propagation of the input value across the network
 *
 * PRE: net != NULL
 *	weights in net have been initialized
 *	ptron_set_input(net)
 *
 * POS: result == PTRON_OK  &&  values successfully propagated
 *	or
 *	result == PTRON_ERR
 */
int
ptron_fwd_prop (ptron_t net)
{
	int res = PTRON_OK;
	int	m = 0,	/* layers iterator	*/
		j = 0,	/* m+1 layer iterator	*/
		i = 0;	/*  m  layer iterator	*/
	double sum = 0.0;
	double (*g) (double) = NULL;
	
	assert (net != NULL);
	assert (net->ready);
	g = net->g;
	
	debug ("%s\n","About to perform forward propagation");
	
	for (m=0 ; m < net->A ; m++) {
		#pragma omp parallel for default(shared) private(i,j,sum)
		for (j=0 ; j < net->N[m+1] ; j++) {
			
			/* sum = Σ w[m][i,j] * V[m][i] */
			sum = 0.0;
			for (i=0 ; i < net->N[m] ; i++)
				sum += net->w[m][j + i*net->N[m+1]] *
					g(net->V[m][i]);
				/* w[m] rows    represent layer m   *
				 * w[m] columns represent layer m+1 */
			
			net->V[m+1][j] = sum;
			/* NOTE: V[m][j] holds the h[m][j] raw value
			 *	 without the net's function g(x) evaluation.
			 *	 Thus to get the real V[m][j] value then
			 *	 g(V[m][j]) must be used
			 */
		}
	}
	
	debug ("%s\n", "Produced output:");
	dfor (j=0 ; j < net->N[net->A] ; j++) {
		debug ("O[%d] = %f\n", j, net->V[net->A][j]);
	}
	
	net->fwd = 1;
	
	return res;
}




/* Performs backwards propagation calculations after a processed sample.
 * NU holds the expected output for the input given prior to the last
 * forward propagation invoked on net.
 *
 * This can be done (successfully) only once after each forward propagation.
 * "gp" should be the derivative of the function the net was created with.
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
 *	result == PTRON_ERR  &&  ptron_fwd_prop(net) must be invoked beforehand
 */
int
ptron_back_prop (ptron_t net, double *NU, double (*gp) (double))
{
	int m = 0, j = 0, i = 0;
	double sum = 0.0;
	double (*g) (double) = NULL;
	static unsigned long sample = 0;
	
	assert (net != NULL);
	assert (NU  != NULL);
	
	if (!net->fwd)
		return PTRON_ERR;
	else
		net->fwd = 0;
	
	debug ("Performing back-propagation for sample # %lu\n", ++sample);
	
	g = net->g;
	m = net->A-1;
	
	/* Computing the deltas for the output layer */
	#pragma omp parallel for default(shared) private(i)
	for (i=0 ; i < net->N[m+1] ; i++) {
		net->delta[m][i] = gp (net->V[m+1][i]) *
					(NU[i] - g (net->V[m+1][i]));
		/* Remember net->delta has length net->N[m+1] */
	}
	
	/* Recursively (backwards) computing the deltas for each layer */
	for (m = net->A-1 ; m > 0 ; m--) {
		
		#pragma omp parallel for default(shared) private(i,j,sum)
		for (i=0 ; i < net->N[m] ; i++) {
			
			/* sum = Σ w[m][i,j] * δ[m][j] */
			sum = 0.0;
			for (j=0 ; j < net->N[m+1] ; j++) {
				
				sum += net->w[m][j + i*net->N[m+1]] *
					net->delta[m][j];
			}
			/* δ[m-1][i] = gp (h[m][i]) * sum    */
			net->delta[m-1][i] = gp (net->V[m][i]) * sum;
		}
	}
	
	/* Computing parallely all weights updates */
	#pragma omp parallel for default(shared) private(m,j,i)
	for (m=0 ; m < net->A ; m++) {
		for (j=0 ; j < net->N[m+1] ; j++) {
			for (i=0 ; i < net->N[m] ; i++) {
				net->dw[m][j + i*net->N[m+1]] += net->etha *
					net->delta[m][j] * net->V[m][i];
			}
		}
	}
	
	debug("%s","Weight updates generated:");
	dfor (m=0 ; m < net->A ; m++) {
		debug ("\nLayer # %d\n", m);
		dfor (j=0 ; j < net->N[m+1] ; j++) {
			dfor (i=0 ; i < net->N[m] ; i++) {
				debug ("dw[%d][%d,%d] = %f\t", m, i, j,
				       net->dw[m][j + i*net->N[m+1]]);
			}
		}
	}
	
	return PTRON_OK;
}




int
ptron_do_updates (ptron_t net)
{
	printbits (0);
	return 0;
}


/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/
