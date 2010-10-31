/*
 * Neural Networks - Deterministic Hopfield Model with Strong Dilution
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 31/10/2010 21:32 $
 * $ License: GPL v3 $
 *
 */


#ifndef STRONG_DILUTION_HOPFIELD_NETWORK
#define STRONG_DILUTION_HOPFIELD_NETWORK


typedef struct _sdhn_s * sdhn_t;


/** ### ### ### ~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Creates an instance of the ADT
 *
 * PARAMETERS:	n = # of neurons in the network
 *		p = # of stored memories
 *		k = mean conectivity (how many neighbours for each neuron)
 *
 * PRE:  k < ln(n)   ,    where ln(x) == "natural logarithm of x"
 *
 * POS: result != NULL
 */
sdhn_t
sdhn_create (unsigned int n, unsigned int p, unsigned int k);


/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = sdhn_destroy (net);
 */
sdhn_t
sdhn_destroy (sdhn_t net);



/** ### ### ### ~~~~~~~~~~ NETWORK INITIALIZERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Sets the # of steps for which the network will relax (ie: run untraced)
 * before overlap measurements begin
 *
 * PRE: net != NULL
 */
void
sdhn_set_untraced (sdhn_t net, unsigned int untraced);


/* Sets the # of times the network will update itself measuring overlaps
 * (ie: run traced) before the average overlap is finally returned
 *
 * PRE: net != NULL
 */
void
sdhn_set_traced (sdhn_t net, unsigned int traced);


/* Initializes randomly all the stored memories in 'net'
 *
 * PRE: net != NULL
 */
void
sdhn_init_XI (sdhn_t net);


/* Initializes randomly all neurons' neighbours in 'net'
 *
 * PRE: net != NULL
 */
void
sdhn_init_neigh (sdhn_t net);


/* Initializes all connection weights in 'net'
 *
 * PRE: net != NULL
 *	sdhn_init_XI    was previously used on net
 *	sdhn_init_neigh was previously used on net
 */
void
sdhn_init_w (sdhn_t net);


/* Initializes the state of the network over the stored memory XI[nu]
 *
 * PRE: net != NULL
 *	sdhn_init_XI  was previously used on net
 *	0 <= nu <= # of memories in net
 *
 * POS: S == XI[nu]
 */
void
sdhn_init_S (sdhn_t net, unsigned int nu);



/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Updates net for <untraced> steps without measuring anything.
 * Then updates net for <traced> steps measuring overlaps between S and XI[nu].
 * Returns the average overlap measured in the <traced> final steps.
 *
 * PRE: net != NULL
 *	sdhn_init_w  was previously used on net
 *	nu ∈ {0,...,p-1}  where p is the # of stored memories in net
 *
 * USE: m = sdhn_run_net (net, nu)
 *
 * POS: m == average overlap between the state S and the memory XI[nu]
 */
double
sdhn_run_net (sdhn_t net, unsigned int nu);



#endif
