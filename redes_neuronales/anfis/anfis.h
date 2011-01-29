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


/* Will use the GNU Scientific Library for algebraic computation */
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>


typedef struct _anfis_s *anfis_t;


/* Functions return values */
#define  ANFIS_OK	( 0)
#define  ANFIS_ERR	(-1)


/* Quantity of possible membership functions (ie: length of MF_kind enum) */
#define  MAX_FUNC	4
/* Maximum # of parameters for any membership function */
#define  MAX_PARAM	4
/* Maximum length for any membership function's name */
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
/**/   } MF_t;                                                              /**/
/**/                                                                        /**/
/******************************************************************************/


/******   Training sample TAD   ***********************************************/
/**/                                                                        /**/
/**/   typedef struct {                                                     /**/
/**/                    gsl_vector *in;                                     /**/
/**/                    double out;                                         /**/
/**/   } t_sample;                                                          /**/
/**/                                                                        /**/
/******************************************************************************/



/**###############    CREATE / DESTROY FUNCTIONS    ##########################*/


anfis_t
anfis_create (size_t n, size_t t, const MF_t *mf);

/* Creates an instance of the ADT
 *
 * PARAMETERS:	n ---> input dimension
 *		t ---> # of membership functions for each input element
 *		mf --> membership functions (must be a n*t vector
 *			representing a n-row,t-column matrix)
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



/**#####################    ACCESSORS / OBSERVERS    #########################*/


void
anfis_print (anfis_t net);

/* Prints into STDOUT the internal state of the network
 * PRE: net != NULL
 */



int
anfis_get_MF (anfis_t net, size_t i, size_t j, MF_t *mf);

/* Copies into 'mf' the network's j-th order membership function,
 * which evaluates the i-th input component (aka: MF[i][j])
 *
 * PRE: net != NULL
 *	mf  != NULL
 *	i < network's n_value (input dimension)
 *	j < network's t_value
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] data has been copied inside 'mf'
 *	or
 *	result == ANFIS_ERR
 */



int
anfis_set_MF (anfis_t net, size_t i, size_t j, MF_t mf);

/* Sets 'mf' as the new network j-th order membership function,
 * which evaluates the i-th input component (aka: MF[i][j])
 *
 * PRE:	net != NULL
 *	i < network's n_value (input dimension)
 *	j < network's t_value
 *
 * POS: result == ANFIS_OK   &&   MF[i][j] has been replaced with 'mf'
 *	or
 *	result == ANFIS_ERR
 */



double *
anfis_get_P (anfis_t net, size_t i);

/* Returns a copy of the network's i-th consequent parameters
 * Caller owns the memory generated for the returned vector,
 * which must be freed using glibc standard free() routine
 *
 * PRE:	net   != NULL
 *	i < t^n , where (n,t) == network's (n_value,t_value)
 *	
 * POS: result != NULL   &&   result contains the coefficients of p[i]
 *	or
 *	result == NULL
 */



int
anfis_set_P (anfis_t net, size_t i, const double *new_p);

/* Sets 'new_p' as the new network i-th row consequent parameters (aka: p[i])
 *
 * PRE:	net   != NULL
 *	new_p != NULL
 *	length_of (new_p) == network's n_value + 1
 *	i < t^n , where (n,t) == network's (n_value,t_value)
 *	
 * POS: result == ANFIS_OK   &&   coefficients in p[i] have been replaced
 *				  with those in 'new_p'
 *	or
 *	result == ANFIS_ERR
 */



/**#######################    NETWORK DINAMICS    ############################*/


int
anfis_train (anfis_t net, t_sample *s, size_t P);

/* Trains the network using the 'P' training samples provided
 * Input of every sample element should have the correct dimension
 * P should greatly outrange the dimension of the input.
 *
 * NOTE: batch-like update mode is used
 * WARNING: this routine is EXTREMELY TIME, MEMORY AND CPU CONSUMING
 *
 * PRE: net != NULL
 *	s   != NULL
 *	length_of (s) == P
 *	length_of (s[k].in) == network's n_value (ie: input dimension)
 *	P > (t^n)*(n+1) , where (n,t) == network's (n_value,t_value)
 *
 * POS:	result == ANFIS_OK   &&   net's parameters have been updated
 *				  sample was not modified
 *	or
 *	result == ANFIS_ERR
 */



double
anfis_eval (anfis_t net, const gsl_vector *input);

/* Feeds the network with the given input. Returns the network produced output
 *
 * PRE: net   != NULL
 *	input != NULL
 *	length_of (input) == network input dimension
 *
 * POS: result == network output for the given input
 */



#endif
