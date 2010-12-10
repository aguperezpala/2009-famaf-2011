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

/* I/O data type */
typedef enum {
	ptron_int,
	ptron_uint,
	ptron_long,
	ptron_ulong,
	ptron_float,
	ptron_double
} io_dtype;


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
ptron_reinit (ptron_t net, double downBound, double upBound);

/* Restarts the sinaptic weights to random values between bounds given
 *
 * PRE: net != NULL
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


unsigned int
ptron_get_layers_size (ptron_t net, unsigned int *N);

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
ptron_set_input (ptron_t net, const void **XI, io_dtype type);

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
ptron_get_output (ptron_t net, void **O, io_dtype type);

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
ptron_back_prop (ptron_t net);

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
ptron_do_updates (ptron_t net);

/* Applies the stored update values to the sinaptic weights
 *
 * PRE: net != NULL
 * POS: result == PTRON_OK  &&  updates successfully applied
 *	or
 *	result == PTRON_ERR
*/


#endif
