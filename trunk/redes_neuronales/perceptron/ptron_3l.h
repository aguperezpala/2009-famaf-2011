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

#ifndef PTRON_H
#define PTRON_H

typedef struct _ptron3_s *ptron3_t;

/* # of layers of the network (including input and output layer) */
#define  NLAYERS	3

/* Functions return values */
#define  PTRON_OK	( 0)
#define  PTRON_ERR	(-1)


/** ### ### ### ~~~~~~~ SUPER-HARDCODED SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* β coefficient for g(x) == tanh (βx)
 * g is the network internal function */
#define  T		0.1
#define  B		1.0/T

/* Network internal function (for both the hidden and the output layer)
 * Changes in g must be reflected in the source code of functions
 * ptron_fwd_prop() and ptron_back_prop()
 */
#define  g(x)  tanh(B*x)



/** ### ### ### ~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


ptron3_t
ptron_create (const unsigned int *N, double etha, double (*f) (double));

/* Creates an instance of the ADT
 *
 * PARAMETERS:	N[i] --> # of neurons of the i-th layer.
 *			 N ranges from 0 (input layer) to NLAYERS (output layer)
 *		etha --> proportionality constant for the gradient descent learn
 *		f -----> sinaptic function
 *
 * PRE: N != NULL
 * POS: result != NULL
 *
 * NOTE: In the current implementation function 'f' is ignored
 *	 Predefined function g(x) is used instead
 */



ptron3_t
ptron_destroy (ptron3_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net);
 */




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


int
ptron_reinit (ptron3_t net, double lowBound, double upBound);

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
ptron_get_layers_size (ptron3_t net, unsigned int N[A]);

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
ptron_get_output (ptron3_t net, double *O);

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
ptron_clear_updates (ptron3_t net);

/* Erases weight updates calculations (aka: delta_w) stored in the network,
 * setting all of them to zero.
 * PRE: net != NULL
 */




/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


int
ptron_fwd_prop (ptron3_t net, const double *XI, size_t len);

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
ptron_back_prop (ptron3_t net, double *NU, double (*gp) (double));

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
ptron_do_updates (ptron3_t net);

/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/


#endif
