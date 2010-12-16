/*
 * Neural Networks - Multi-layer perceptron
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 09/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */

#ifndef PTRON_H
#define PTRON_H


typedef struct _ptron_s *ptron_t;

/* Functions return value */
#define PTRON_OK  ( 0)
#define PTRON_ERR (-1)



/** ### ### ### ~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


ptron_t
ptron_create (unsigned int A, const unsigned int *N, double (*g) (double));

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
ptron_destroy (ptron_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net);
 */



/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


int
ptron_reinit (ptron_t net, double lowBound, double upBound);

/* Restarts the sinaptic weights to random values between bounds given
 *
 * PRE: net != NULL
 *	upBound > lowBound
 *
 * POS: result == PTRON_OK  &&  sinaptic weights reinitialized
 *	or
 *	result == PTRON_ERR
 */


unsigned int
ptron_get_num_layers (ptron_t net);

/* Returns the number of layers the network was created with,
 * including both input and output layer.
 *
 * PRE: net != NULL
 */


unsigned int *
ptron_get_layers_size (ptron_t net, unsigned int *N);

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


int
ptron_set_input (ptron_t net, const double *XI, size_t length);

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
ptron_fwd_prop (ptron_t net);

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
ptron_back_prop (ptron_t net, double *NU, double (*gp) (double));

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
ptron_do_updates (ptron_t net);

/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/


#endif
