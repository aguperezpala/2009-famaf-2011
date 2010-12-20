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


/* Maximum # of parameters for any membership function */
#define  MAX_PARAM	4

typedef enum {
	triang,
	trap,
	gauss,
	bell,
	exp
} MF_kind;

typedef struct {
	MF_kind   kind;			/* Membership function's kind */
	double    param[MAX_PARAMS];	/* Membership function's parameters */
} MF_t;



/**###############    CREATE / DESTROY FUNCTIONS    ##########################*/


anfis_t
anfis_create (int n, int t, MF_t *mf);

/* Creates an instance of the ADT
 *
 * PARAMETERS:	n ----> input dimension
 *		t ----> # of branches the network will have
 *		mf ---> membership functions. This must be a vector
 *			of length [t]x[n], simulating a [t][n] matrix
 *
 * PRE: mf != NULL
 * POS: result != NULL
 */



anfis_t
anfis_destroy (anfis_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = anfis_destroy (net);
 */


#endif
