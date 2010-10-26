/*
 * Neural Networks - Deterministic Hopfield Model with Strong Dilution
 * Network ADT
 *
 * NOTE
 * All functions consider a BITWISE STORAGE within vectors and matrixes.
 * That is, each element is considered to occupy one bit of memory space.
 * ie: for each S[i] or XI[mu][i] position there are X stored components,
 * where X = sizeof(long). Usually X = 64.
 */


#ifndef STRONG_DILUTION_HOPFIELD_NETWORK
#define STRONG_DILUTION_HOPFIELD_NETWORK


typedef struct _sdhn_s * sdhn_t;


/** ~~~~~~~~~~~~~~~~~~ CREATE / DESTROY FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


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
sdhn_destroy (sdnh_t net);



/** ~~~~~~~~~~~~~~~~~~~~~~ NETWORK INITIALIZERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Sets the # of steps for which the network will relax (ie: run untraced)
 * before overlap measurements begin
 *
 * PRE: net != NULL
 */
void
sdhn_set_untraced (sdhn_t net);


/* Sets the # of updates for which the network will run measuring overlaps(ie: run untraced)
 * before overlap measurements begin
 *
 * PRE: net != NULL
 */
void
sdhn_set_traced (sdhn_t net);


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



/** ~~~~~~~~~~~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */



#endif
