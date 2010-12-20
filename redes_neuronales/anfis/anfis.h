/*
 * Adaptive-Network-Based Fuzzy Inference System (ANFIS)
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 13/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */

#ifndef _ANFIS_H
#define _ANFIS_H


typedef struct _anfis_s *anfis_t;


/* Quantity of possible membership functions (ie: length of MF_kind enum) */
#define  MAX_FUNC	4
/* Maximum # of parameters for any membership function */
#define  MAX_PARAM	4

typedef enum {
	triang,
	trap,
	gauss,
	bell
} MF_kind;

typedef struct {
	MF_kind   k ;		   /* Membership function's kind */
	double    p [MAX_PARAM];   /* Membership function's parameters */
} MF;



/**###############    CREATE / DESTROY FUNCTIONS    ##########################*/


anfis_t
anfis_create (unsigned long n, unsigned long t, const MF *a);

/* Creates an instance of the ADT
 *
 * PARAMETERS:	n ---> input dimension
 *		t ---> # of branches the network will have
 *		a ---> membership functions. This must be a vector
 *			of length [t]x[n], simulating a [t][n] matrix
 *
 * PRE: a != NULL
 * POS: result != NULL
 */



anfis_t
anfis_destroy (anfis_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = anfis_destroy (net);
 */



/** ### ### ### ~~~~~~~~~ NETWORK INITIALIZERS/OBSERVERS ~~~~~~~~~~~~~~~~~~~~ */


void
anfis_print (anfis_t net);

/* Prints in STDOUT the internal state of the network
 * PRE: net != NULL
 */


#endif
