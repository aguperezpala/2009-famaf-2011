/*
 * Neural Networks - Deterministic Hopfield Model with Strong Dilution
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 25/10/2010 21:32 $
 * $ License: GPL v3 $
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "sdhn.h"
#include "mzran13.h"


#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long))


/* Debugging printing function */
#ifdef _DEBUG
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__); fflush(stderr)
  #define  dfor(s)	 for(s)
#else
  #define  debug(s,...)
  #define  dfor(s)
#endif


/* Strong Dilution Hopfield Network ADT */
struct _sdhn_s {
	int n;		/* # of neurons 		*/
	int p;		/* # of memories		*/
	int k;		/* # of neighbours per neuron	*/
	long traced;	/* # of traced steps per run	*/
	long untraced;	/* # of untraced steps per run	*/
	long *S;	/* network state		*/
	long *XI;	/* stored memories		*/
	long *neigh;	/* neighbours matrix		*/
	long *w;	/* connection weights matrix	*/
	short XI_init;	/* Whether memories were initialized	*/
	short neigh_init;	/* Whether neighbours were initialized	*/
	short w_init;	/* Whether connection weights were init */
};

/* NOTE
 * BITWISE STORAGE is used for states in 'S' and stored memories in 'XI'.
 * That is, each element in those vectors occupies one bit of memory space.
 * ie: for each S[i] or XI[mu][i] position there are M stored components,
 * where M = sizeof(long). Usually X = 64.
 */



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
sdhn_t sdhn_create (unsigned int n, unsigned int p, unsigned int k)
{
	sdhn_t net = NULL;
	unsigned long N = 0;
	
	assert ((double) k <= log ((double) n));
	
	net = (sdhn_t) malloc (sizeof (struct _sdhn_s *));
	assert (net != NULL);
	
	net->n = n & (~(((unsigned long) 1) << (MSB-1)));
	net->p = p & (~(((unsigned long) 1) << (MSB-1)));
	net->k = k & (~(((unsigned long) 1) << (MSB-1)));
	
	/* Remember work will be bitwise in S and in XI */
	N = n/MSB + !(!(n%MSB));
	
	net->S  = (long *) calloc (N,   sizeof(long));
	net->XI = (long *) calloc (N*p, sizeof(long));
	net->w  = (long *) calloc (n*k, sizeof(long));
	net->neigh = (long *) calloc (n*k, sizeof(long));
	
	assert (net->S  != NULL);
	assert (net->XI != NULL);
	assert (net->w  != NULL);
	assert (net->neigh != NULL);
	
	net->XI_init = 0;
	net->neigh_init = 0;
	net->w_init = 0;
	
	return net;
}



/* Destroys the ADT and frees its memory resources
 * PRE: net != NULL
 * USE: net = sdhn_destroy (net);
 */
sdhn_t
sdhn_destroy (sdhn_t net)
{
	assert (net != NULL);
	
	free (net->S);		net->S = NULL;
	free (net->XI);		net->XI = NULL;
	free (net->w);		net->w = NULL;
	free (net->neigh);	net->neigh = NULL;
	free (net);		net = NULL;
	
	return net;
}



/** ### ### ### ~~~~~~~~~~ NETWORK INITIALIZERS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Sets the # of steps for which the network will relax (ie: run untraced)
 * before overlap measurements begin
 *
 * PRE: net != NULL
 */
void
sdhn_set_untraced (sdhn_t net, unsigned int untraced)
{
	assert (net != NULL);
	net->untraced = untraced;
}


/* Sets the # of times the network will update itself measuring overlaps
 * (ie: run traced) before the average overlap is finally returned
 *
 * PRE: net != NULL
 */
void
sdhn_set_traced (sdhn_t net, unsigned int traced)
{
	assert (net != NULL);
	net->traced = traced;
}


/* Initializes randomly all the stored memories in 'net'
 *
 * PRE: net != NULL
 */
void
sdhn_init_XI (sdhn_t net)
{
	long i = 0, size = 0;
	
	assert (net != NULL);
	
	size = net->p * (net->n / MSB + !(!(net->n % MSB)));
	
	/* NOTE With #pragma statement this section becomes non-deterministic,
	 * as the output memory XI[i] will depend on the execution order of the
	 * execution threads, which itself is non-deterministic.
	 *	To regain determinism in the creation of the XI matrix simply
	 * comment the #pragma statement
	 * WARNING mzran13 is NOT threadsafe
	 */
/*	#pragma omp parallel for shared(XI)
*/	for (i=0 ; i<size ; i++) {
		net->XI[i] = (long) mzran13();
	}
	
	net->XI_init = 1;
	
	return;
}



/* Chooses a fresh j-th neighbour for the i-th neuron in net */
static void
choose_fresh_neigh (sdhn_t net, long i, long j)
{
	short fresh = 0;
	long cand = 0, l = 0;
	long *ne = net->neigh;
	
	while (!fresh) {
		/* Get a candidate */
		cand = mzran13() % net->n;
		fresh = 1;
		/* Check if it's not in the list yet */
		for (l=0 ; l<j ; l++) {
			if ( cand == ne[l+i*net->k] ) {
				fresh = 0;
			}
		}
	}
	/* Set the new neighbour */
	ne[j+i*net->k] = cand;
	
	return;
}


/* Initializes randomly all neurons' neighbours in 'net'
 *
 * PRE: net != NULL
 */
void
sdhn_init_neigh (sdhn_t net)
{
	long i = 0, j = 0;
	
	assert (net != NULL);
	
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < net->n ; i++) {
		for (j=0 ; j < net->k ; j++)
			choose_fresh_neigh (net, i, j);
	}
	
	net->neigh_init = 1;
	
	debug ("%s","Neighbours list:\n");
	dfor (i=0 ; i < net->n ; i++) {
		debug ("%ld)\t", i);
		dfor (j=0 ; j < net->k ; j++)
			debug ("%ld\t", net->neigh[j+i*net->k]);
		debug ("%s","\n");
	}
	
	return;
}


/* Initializes all connection weights in 'net'
*
* PRE: net != NULL
*	sdhn_init_XI    was previously used on net
*	sdhn_init_neigh was previously used on net
*/
void
sdhn_init_w (sdhn_t net)
{
	return;
}


/* Initializes the state of the network over the stored memory XI[nu]
*
* PRE: net != NULL
*	sdhn_init_XI  was previously used on net
*
* POS: S == XI[nu]
*/
void
sdhn_init_S (sdhn_t net, unsigned int nu)
{
	return;
}



/** ~~~~~~~~~~~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


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
sdhn_run_net (sdhn_t net, unsigned int nu)
{
	return 0.0;
}

