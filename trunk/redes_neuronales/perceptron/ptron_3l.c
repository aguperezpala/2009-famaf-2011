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

#include "ptron.h"
#include "mzran13.h"

struct _ptron3_s {
	int *N;			/* # of neurons per layer*/
	double etha;		/* updates scaling parameter */
	
	double *V;		/* Middle layer neurons */
	double *O;		/* Output layer neurons */
	
	double *w1;		/* Weights between input  & middle layers */
	double *w2;		/* Weights between middle & output layers */
	
	double *dw1;		/* Back-propagation updates for w1 */
	double *dw2;		/* Back-propagation updates for w2 */
	double *d1;		/* Auxiliary values for dw1 calculation */
	double *d2;		/* Auxiliary values for dw2 calculation */
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
 *			 N ranges from 0 (input layer) to 2 (output layer)
 *		etha --> proportionality constant for the gradient descent learn
 *		f -----> sinaptic update function
 *
 * PRE: N != NULL
 * POS: result != NULL
 *
 * NOTE: In the current implementation function 'f' is ignored
 *	 Predefined function g(x) is used instead
 */
ptron_t
ptron_create (const unsigned int *N, double etha, double (*f) (double))
{
	ptron
	
	return net;
}




ptron_t
ptron_destroy (ptron_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net);
 */




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


int
ptron_reinit (ptron_t net, double lowBound, double upBound);

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
ptron_get_layers_size (ptron_t net, unsigned int N[A]);

/* Stores in N the # of neurons of each layer in th network,
 * including both input and output layers.
 *
 * PRE: net != NULL
 *
 * POS: result == PTRON_OK  &&  sizes stored in N
 *	or
 *	result == PTRON_ERR
 */



double *
ptron_get_output (ptron_t net, double *O);

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



void
ptron_clear_updates (ptron_t net);

/* Erases weight updates calculations (aka: delta_w) stored in the network,
 * setting all of them to zero.
 * PRE: net != NULL
 */




/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


int
ptron_fwd_prop (ptron_t net, const double *XI, size_t len);

/* Performs forward propagation of the input value "XI" across the network.
 * XI is of length "len", and at most length_of(input-layer) elements of it
 * will be used in the propagation.
 *
 * PRE: net != NULL
 *	weights in net have been initialized
 *	XI != NULL
 *
 * POS: result == PTRON_OK  &&  values successfully propagated
 *	or
 *	result == PTRON_ERR
 */



int
ptron_back_prop (ptron_t net, double *NU, double (*gp) (double));

/* Performs backwards propagation calculations after a processed sample.
 * NU holds the expected output for the input given to perform the last
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
ptron_do_updates (ptron_t net);

/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/


#endif
