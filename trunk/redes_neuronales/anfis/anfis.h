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


/* Functions return values */
#define  ANFIS_OK	( 0)
#define  ANFIS_ERR	(-1)



/* Quantity of possible membership functions (ie: length of MF_kind enum) */
#define  MAX_FUNC	4
/* Maximum # of parameters for any membership function */
#define  MAX_PARAM	4
/* Maximum length for any membership function */
#define  MAX_NLEN	25


/******   Membership function TAD   *******************************************/
/**/                                                                        /**/
/**/   typedef enum {                                                       /**/
/**/                  triang,                                               /**/
/**/                  trap,                                                 /**/
/**/                  gauss,                                                /**/
/**/                  bell                                                  /**/
/**/   } MF_kind;                                                           /**/
/**/                                                                        /**/
/**/   typedef struct {                                                     /**/
/**/                    MF_kind  k ;             /* MF's kind */            /**/
/**/                    double   p [MAX_PARAM];  /* MF's parameters */      /**/
/**/   } MF;                                                                /**/
/**/                                                                        /**/
/******************************************************************************/


/******   Training sample TAD   ***********************************************/
/**/                                                                        /**/
/**/   typedef struct {                                                     /**/
/**/                    double *in;                                         /**/
/**/                    double out;                                         /**/
/**/   } t_sample;                                                          /**/
/**/                                                                        /**/
/******************************************************************************/


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



/**#####################    ACCESSORS / OBSERVERS    #########################*/


void
anfis_print (anfis_t net);

/* Prints into STDOUT the internal state of the network
 * PRE: net != NULL
 */



void
anfis_print_branch (anfis_t net, unsigned int i);

/* Prints into STDOUT the internal state of the network's i-th branch
 * PRE: net != NULL
 *	i < network # of branches
 */



int
anfis_get_MF (anfis_t net, unsigned int i, unsigned int j, MF *a);

/* Copies into 'a' the network's i-th row membership function,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE: net != NULL
 *	a   != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] data has been copied inside 'a'
 *	or
 *	result == ANFIS_ERR
 */



int
anfis_set_MF (anfis_t net, unsigned int i, unsigned int j, MF a);

/* Sets 'a' as the new network membership function in the i-th row,
 * which evaluates the j-th input component (aka: MF[i][j])
 *
 * PRE:	net != NULL
 *	i < network # of branches
 *	j < network input dimension
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] has been replaced with 'a'
 *	or
 *	result == ANFIS_ERR
 */



double *
anfis_get_P (anfis_t net, unsigned int i);

/* Returns a copy of the network's i-th consequent parameters
 * Caller owns the memory generated for the returned vector,
 * which must be freed using glibc standard free() routine
 *
 * PRE:	net   != NULL
 *	i < network # of branches
 *	
 * POS: result != NULL   &&   result contains the coefficients of p[i]
 *	or
 *	result == NULL
 */



int
anfis_set_P (anfis_t net, unsigned int i, const double *new_p);

/* Sets 'new_p' as the new network i-th row consequent parameters (aka: p[i])
 *
 * PRE:	net   != NULL
 *	new_p != NULL
 *	length_of (new_p) == network input dimension + 1
 *	i < network # of branches
 *	
 * POS: result == ANFIS_OK   &&   coefficients in p[i] have been replaced
 *				  with those in 'new_p'
 *	or
 *	result == ANFIS_ERR
 */



/**#######################    NETWORK DINAMICS    ############################*/


int
anfis_train (anfis_t net, unsigned int P, t_sample *sample);

/* Trains the network using the 'P' training samples provided
 * Input of every sample element should have the correct dimension
 *
 * NOTE: batch-like update mode is used
 * WARNING: this routine is EXTREMELY slow and CPU bound
 *
 * PRE: net != NULL
 *	sample != NULL
 *	length_of (sample) == P
 *	length_of (sample[k].in) == network input dimension
 *
 * POS:	result == ANFIS_OK   &&   net's parameters have been updated
 *	or
 *	result == ANFIS_ERR
 */



double
anfis_eval (anfis_t net, double *input);

/* Feeds the network with the given input. Returns the network final output
 *
 * PRE: net != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == network output for the given input
 */


#endif
