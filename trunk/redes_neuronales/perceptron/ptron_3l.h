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

/* Network "adaptive parameters mode" coefficients */
#define  INCREASE	0.02
#define  DECREASE	0.1

/* Net update modes */
typedef enum {
	std,	/* Standard */
	mom,	/* Momentum */
	adp,	/* Standard + adaptive parameters */
	full	/* Momentum + adaptive parameters */
} ptron_dynamic;


/** ### ### ### ~~~~~~~ SUPER-HARDCODED SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define  T		0.1
#define  B		1.0/T
/* β coefficient for g(x) == tanh (βx)
 * g is the network internal function */

#define  g(x)		tanh(B*x)
/* Network internal function (for both the hidden and the output layer)
 * Changes in g must be reflected in the source code of functions
 * ptron_fwd_prop() and ptron_back_prop()
 */



/** ### ### ### ~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


ptron3_t
ptron_create (const unsigned int N[NLAYERS], double etha, double alpha,
	      double (*f) (double), double (*df) (double));

/* Creates an instance of the ADT
 *
 * PARAMETERS:	N[i] --> # of neurons of the i-th layer.
 *			 N ranges from 0 (input layer) to NLAYERS-1 (output l.)
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
ptron_destroy (ptron3_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = ptron_destroy (net);
 */




/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


int
ptron_reinit_weights (ptron3_t net, double lowBound, double upBound);

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



void
ptron_get_layers_size (ptron3_t net, unsigned int N[NLAYERS]);

/* Stores in N the # of neurons of each layer in th network,
 * including both input and output layers.
 *
 * PRE: net != NULL
 *
 * POS: sizes stored in N
 */



double *
ptron_get_output (ptron3_t net);

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



double
ptron_calc_err (ptron3_t net, double *NU);

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




double *
ptron_get_err (ptron3_t net);

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




void
ptron_reset (ptron3_t net);

/* Erases weight updates (aka: delta_w) and learning error values
 * stored in the network, setting them all to 0
 * PRE: net != NULL
 */




/** ### ### ### ~~~~~~~~~ NETWORK DYNAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


int
ptron_fwd_prop (ptron3_t net, const double *XI, size_t len);

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
ptron_back_prop (ptron3_t net, double *NU, ptron_dynamic mode);

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




void
ptron_do_updates (ptron3_t net);

/* Applies the stored update values to the sinaptic weights,
 * according to the update mode specified.
 *
 * PRE: net != NULL
 * POS: network sinaptic weights updated
 */


#endif
