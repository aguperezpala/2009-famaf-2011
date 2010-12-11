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
	double **V;		/* neurons */
	double **w;		/* sinaptic weights matrix */
	double (*g) (double);	/* propagation function */
	double **dw;		/* for back-propagation updates */
};


#define  INPUT  0


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
	
	/* update deltas
	 * net->dw[i] holds the update for the weights in net->w[i]
	 * For that reason net->dw[i] ranges in [ 0 , net->N[i+1] )
	 */
	net->dw = (double **) malloc (net->A * sizeof (double *));
	assert (net->dw != NULL);
	for (i=0 ; i < net->A ; i++) {
		net->dw[i] = (double *) malloc (net->N[i+1] * sizeof (double));
		assert (net->dw[i] != NULL);
	}
	
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
		free (net->dw[i]);	net->dw[i] = NULL;
	}
	free (net->V[net->A]);	net->V[net->A] = NULL;
	
	free (net->V);	net->V = NULL;
	free (net->w);	net->w = NULL;
	free (net->dw);	net->dw = NULL;
	
	free(net);	net = NULL;
	
	return net;
}




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


/* Restarts the sinaptic weights to random values between bounds given
 *
 * PRE: net != NULL
 *	upBound > downBound
 *
 * POS: result == PTRON_OK  &&  sinaptic weights reinitialized
 *	or
 *	result == PTRON_ERR
 */
int
ptron_reinit (ptron_t net, double downBound, double upBound)
{
	int res = PTRON_OK;
	int i = 0, j = 0;
	double ran = 0.0;
	
	assert (net != NULL);
	assert (upBound > downBound);
	
	debug("%s","Restarting deltas\n");
	for (i=0 ; i < net->A ; i++) {
		for (j=0 ; j < net->N[i+1] ; j++) {
			ran = ((double) mzran13()) / ((double) ULONG_MAX);
			net->dw[i][j] = ran * (upBound - downBound) + downBound;
#ifdef _DEBUG
			debug("dw[%d][%d] = %.4f\n", i, j, net->dw[i][j]);
			if (net->dw[i][j] < downBound || net->dw[i][j] > upBound)
				res = PTRON_ERR;
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




/* Sets vector XI as the network's new input pattern
 *
 * PARAMETERS:	net -----> perceptron network
 *		XI ------> input array
 *		length --> number of elements in XI
 *		n -------> size (in bytes) of each element in XI
 *
 * PRE: net != NULL
 *	XI  != NULL
 *
 * POS: result == PTRON_OK  &&  net's input layer == XI
 *	or
 *	result == PTRON_ERR
 */
int
ptron_set_input (ptron_t net, const void *XI, size_t length, size_t n)
{
	int i = 0, res = PTRON_OK;
	
	assert (net != NULL);
	assert (XI != NULL);
	
	memcpy (&(net->V[INPUT][0]), XI, n*length);
	
	/** TODO HACER QUE LA MEMORIA SEA COPIADA EN LA LINEA DE ARRIBA */
	
	dfor (i=0 ; i < net->N[INPUT] ; i++) {
		debug("INPUT[%d] = %.3f\n", i, net->V[INPUT][i]);
	}
/*	for (i=0 ; i < net->N[INPUT] ; i++) {
		store_input (&(net->V[INPUT][i]), &(XI[i]), type);
		net->V[INPUT][i] = (double) (((int *) XI)[i]);
		debug("INPUT[%d] = %.3f\n", i, net->V[INPUT][i]);
	}
*/	
	return res;
}




int
ptron_get_output (ptron_t net, void **O, io_dtype type)
{
	printbits (0);
	return 0;
}


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
{
	return 0;
}


/* Erases weight updates calculations (aka: delta_w) stored in the network,
 * setting all of them to zero.
 * PRE: net != NULL
 */



/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


int
ptron_fwd_prop (ptron_t net)
{
	return 0;
}


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
{
	return 0;
}


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
{
	return 0;
}


/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/
