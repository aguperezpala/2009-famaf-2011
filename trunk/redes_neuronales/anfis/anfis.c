/*
 * Adaptive-Network-Based Fuzzy Inference System (ANFIS)
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 13/12/2010 11:35 $
 * $ License: GPL v3 $
 *
 */

#include <stdlib.h>
#inlcude <stdio.h>
#include <assert.h>
#include <math.h>

#include "anfis.h"


#define  MAX(x,y)	((x) > (y) ? (x) : (y))
#define  MIN(x,y)	((x) < (y) ? (x) : (y))

#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long)-1)
#define  int_val(x)	((x) & (~(((unsigned long) 1) << MSB)))

/* Debugging printing functions */
#ifdef _DEBUG
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
#else
  #define  debug(s,...)
  #define  dfor(s)
#endif



/**######################    STRUCTURES & STUFF   ############################*/


typedef struct {
	MF_kind   k ;			/* MF's type */
	double    p [MAX_PARAMS];	/* MF's parameters */
	double (* f ) (double);		/* MF's function */
} MF;

typedef struct {
	MF	*A;	/* membership functions */
	double	*P;	/* polynomial coefficients */
} branch;

struct _anfis_s {
	int n;		/* # of inputs */
	int t;		/* # of branches */
	branch *row;	/* branches */
};

double
triang (double a, double b, double c, double x) {
	return MAX (0.0, MIN ((x-a)/(b-a), (c-x)/(c-b)));
}

double
trap ((double a, double b, double c, double d, double x) {
	return MAX (0.0, MIN ( MIN ((x-a)/(b-a), (d-x)/(d-c)), 1.0) );
})
 gauss,
 bell,
 exp


/**###############    CREATE / DESTROY FUNCTIONS    ##########################*/


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
anfis_create (int n, int t, MF_t *mf)
{
	anfis_t net = NULL;
	
	assert (mf != NULL);
	
	return net;
}




anfis_t
		anfis_destroy (anfis_t net);

/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = anfis_destroy (net);
 */
