/*
 * Neural Networks - Multi-layer perceptron
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 09/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */


#include <stdlib.h>
#include <assert.h>

#include "ptron.h"

struct _ptron_s {
	int A;			/* # of layers (counting input & output) */
	int *N;			/* # of neurons per layer*/
	double **V;		/* neurons */
	double **w;		/* sinaptic weights matrix */
	double (*g) (double);	/* sinaptic update function */
	double **dw;		/* for back-propagation updates */
};



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
  #define printbits(n)
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
	
	/* size of each layer */
	net->N = (int *) calloc (net->A+1, sizeof (int));
	assert (net->N != NULL);
	for (i=0 ; i <= net->A ; i++) {
		net->N[i] = int_val(N[i]);
	}
	
	/* neurons */
	net->V = (double **) malloc (sizeof (double *));
	assert (net->V != NULL);
	for (i=0 ; i <= net->A ; i++) {
		net->V[i] = (double *) malloc (net->N[i] * sizeof (double));
		assert (net->V[i] != NULL);
	}
	
	/* Let 'n' be the i-th layer size, that is: n == net->N[i]
	 * same with 'm' and layer "i+1"
	 * Then w[i] is a [n]x[m] matrix representing sinaptic weights between
	 * layers 'i' and 'i+1'
	 */
	net->w = (double **) malloc (sizeof (double *));
	assert (net->w != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->w[i] = (double *) malloc (net->N[i] * net->N[i+1] *
						sizeof (double));
		assert (net->w[i] != NULL);
	}
	
	/* update function */
	net->g = g;
	
	/* update deltas
	 * net->dw[i] holds the update for the weights in net->w[i] */
	net->dw = (double **) malloc (sizeof (double *));
	assert (net->dw != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->dw[i] = (double *) malloc (net->N[i+1] * sizeof (double));
		assert (net->dw[i] != NULL);
	}
	
	return net;
}




ptron_t
ptron_destroy (ptron_t net)
{}


/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net)
{}

 */



/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


int
ptron_reinit (ptron_t net, double downBound, double upBound)
{}


/* Restarts the sinaptic weights to random values between bounds given
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  sinaptic weights reinitialized
 *	or
 *	result == PTRON_ERR
 */


unsigned int
ptron_get_num_layers (ptron_t net)
{}


/* Returns the number of layers the network was created with,
 * including both input and output layer.
 *
 * PRE: net != NULL
 */


unsigned int
ptron_get_layers_size (ptron_t net, unsigned int *N)
{}


/* Stores in N the # of neurons of each layer in th network,
 * including both input and output layers.
 * Caller owns the memory stored for array N.
 *
 * PRE: net != NULL
 *	N == NULL
 *
 * POS: result == PTRON_OK  &&  sizes stored in N
 *	or
 *	result == PTRON_ERR
 */


int
ptron_set_input (ptron_t net, const void **XI, io_dtype type)
{}


/* Sets vector XI as the network's new input pattern
 *
 * PRE: net != NULL
 *	type(XI) == type
 *	size(XI) == size(net's input layer)
 *
 * POS: result == PTRON_OK  &&  net's input layer == XI
 *	or
 *	result == PTRON_ERR
 */


int
ptron_get_output (ptron_t net, void **O, io_dtype type)
{}


/* Gets the network output layer values
 * Caller owns the memory allocated for vector 'O'
 * Free using standard free()
 *
 * PRE: net != NULL
 *	*O == NULL
 *
 * POS: result == PTRON_OK  &&  *O != NULL
 *	or
 *	result == PTRON_ERR &&  *O == NULL
 */


int
ptron_clear_updates (ptron_t net)
{}


/* Erases weight updates calculations (aka: delta_w) stored in the network,
 * setting all of them to zero.
 * PRE: net != NULL
 */



/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


int
ptron_fwd_prop (ptron_t net)
{}


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
ptron_back_prop (ptron_t net)
{}


/* Performs backwards propagation calculations after a forward propagated sample
 *
 * NOTE: this function computes the next sinaptic weight updates and stores them
 *	 incrementally, but it DOES NOT PERFORM THE ACTUAL UPDATE
 *	
 *	 To do so ptron_do_updates(net) must be invoked
 *	
 *	 "incrementally" means that newly computed updates are added to any
 *	 previous update value in the network.
 *
 * PRE: net != NULL
 *	ptron_fwd_prop(net)
 *
 * POS: result == PTRON_OK  &&  updates successfully computed
 *	or
 *	result == PTRON_ERR
 */


int
ptron_do_updates (ptron_t net)
{}


/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/
