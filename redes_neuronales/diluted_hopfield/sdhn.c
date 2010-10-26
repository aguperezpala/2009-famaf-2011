#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "sdhn.h"



#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long))



/* Strong Dilution Hopfield Network ADT */
struct _sdhn_s {
	int n;		/* # of neurons 		*/
	int p;		/* # of memories		*/
	long *S;	/* network state		*/
	long *XI;	/* stored memories		*/
	long *neigh;	/* neighbours matrix		*/
	long *w;	/* connection weights matrix	*/
	short XI_init = 0;	/* Whether memories were initialized	*/
	short neigh_init = 0;	/* Whether neighbours were initialized	*/
};



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
	
	return net;
}

