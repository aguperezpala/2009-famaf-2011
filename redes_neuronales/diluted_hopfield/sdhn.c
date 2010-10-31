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
#include <omp.h>
#include <assert.h>
#include "sdhn.h"
#include "mzran13.h"



/** ### ### ### ~~~~~~~ CONSTANTS & MACROS  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(long))


/* Debugging printing functions */
#ifdef _DEBUG
  #define  debug(s,...)  fprintf(stderr, s, ##__VA_ARGS__)
  #define  dfor(s)	 for(s)
  static void
  printbits (uint64_t n)
  {
	uint64_t l = ((uint64_t)1) << 63;
	short k = 1;
  	
	while (l > 0) {
		debug ("%c", n&l ? '1' : '0');
		l >>= 1;
		!(k++ % _byte_size) ? debug("%s"," ") : 0;
	}
	debug ("%s","\n");
  }
#else
  #define  debug(s,...)
  #define  dfor(s)
  #define printbits(n)
#endif



/* Counts set bits in x (ie: bits in x equal to '1') */
static inline int
bitcount (unsigned long x)
{
	int b = 0;
	for (b = 0; x != 0; x &= (x-1))
		b++;
	return b;
}



/** ### ### ### ~~~~~~~~~~~~ STRUCTURE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Strong Dilution Hopfield Network ADT */
struct _sdhn_s {
	
	int n;		/* # of neurons 		*/
	int p;		/* # of memories		*/
	int k;		/* # of neighbours per neuron	*/
	long traced;	/* # of traced steps per run	*/
	long untraced;	/* # of untraced steps per run	*/
	
	long *neigh;		/* neighbours matrix		*/
	long *w;		/* connection weights matrix	*/
	unsigned long *S;	/* network state		*/
	unsigned long *SD;	/* duplicate state, for updates */
	unsigned long *XI;	/* stored memories		*/
	
	short XI_init;		/* Whether memories were initialized	*/
	short neigh_init;	/* Whether neighbours were initialized	*/
	short w_init;		/* Whether connection weights were init */
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
	
	net = (sdhn_t) malloc (sizeof (struct _sdhn_s));
	assert (net != NULL);
	
	net->n = n & (~(((unsigned long) 1) << (MSB-1)));
	net->p = p & (~(((unsigned long) 1) << (MSB-1)));
	net->k = k & (~(((unsigned long) 1) << (MSB-1)));
	
	net->neigh = (long *) calloc (n*k, sizeof (long));
	net->w     = (long *) calloc (n*k, sizeof (long));
	
	/* Remember work will be bitwise in S and in XI */
	N = n/MSB + !(!(n%MSB));
	
	net->S  = (unsigned long *) calloc (N,   sizeof (unsigned long));
	net->SD = (unsigned long *) calloc (N,   sizeof (unsigned long));
	net->XI = (unsigned long *) calloc (N*p, sizeof (unsigned long));
	
	assert (net->neigh != NULL);
	assert (net->w  != NULL);
	assert (net->S  != NULL);
	assert (net->SD != NULL);
	assert (net->XI != NULL);
	
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
	
	free (net->neigh);	net->neigh = NULL;
	free (net->w);		net->w  = NULL;
	free (net->S);		net->S  = NULL;
	free (net->SD);		net->SD = NULL;
	free (net->XI);		net->XI = NULL;
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
	long i = 0, N = 0;
	
	assert (net != NULL);
	
	N = net->n / MSB + !(!(net->n % MSB));
	
	/* NOTE With #pragma statement this section becomes non-deterministic,
	 * as the output memory XI[i] will depend on the execution order of the
	 * execution threads, which itself is non-deterministic.
	 *	To regain determinism in the creation of the XI matrix simply
	 * comment the #pragma statement
	 * WARNING mzran13 is NOT threadsafe
	 */
/*	#pragma omp parallel for shared(XI)
*/	for (i=0 ; i < N*net->p ; i++) {
		net->XI[i] = (unsigned long) mzran13();
	}
	
	net->XI_init = 1;
	
	debug ("%s","Memories:\n");
	dfor (i=0 ; i<net->p ; i++) {
		debug ("%ld)", i);
		dfor (int j=0 ; j < N ; j++) {
			debug ("%s","\t");
			printbits(net->XI[j+i*N]);
		}
	}
	debug ("%s","\n");
	
	return;
}



/* Chooses a fresh j-th neighbour for the i-th neuron in net */
static void
choose_fresh_neigh (sdhn_t net, long i, long j)
{
	short fresh = 0;
	long cand = 0, l = 0;
	long *ne = net->neigh;
	
	/* TODO
	 * This method is ~ O(n), and therefore higly inneficient.
	 * The uniqueness of each neighbour should be checked by means of a
	 * hash table or some other method of O(1)
	 */
	
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




/* Calculates and saves the weight w[i][j] */
static void
compute_weight (sdhn_t net, long i, long j)
{
	long N = 0, m = 0, mu = 0, ne = 0, Xi = 0, Xn = 0;
	unsigned long mask = 0;
	long *w = NULL;
	
	/* ne == neigh[i][j] */
	ne = net->neigh[j+i*net->k];
	/*  w == w[i][j] */
	w = &(net->w[j+i*net->k]);
	*w = 0;
	/* Remember work in XI is bitwise */
	N = net->n / MSB + !(!(net->n % MSB));
	
	for (mu=0 ; mu < net->p ; mu++) {
		
		/* Xi = XI[mu][i] */
		mask = ((unsigned long) 1) << (i%MSB);
		m = mu*N + i/MSB;
		Xi = (net->XI[m] & mask) > 0 ? 1 : -1;
		
		/* Xn = XI[mu][ne] */
		mask = ((unsigned long) 1) << (ne%MSB);
		m = mu*N + ne/MSB;
		Xn = (net->XI[m] & mask) > 0 ? 1 : -1;
		
		*w += Xi * Xn;
	}
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
	long i = 0, j = 0;
	
	assert (net != NULL);
	assert (net->XI_init);
	assert (net->neigh_init);
	
	#pragma omp parallel for shared(net) private(i,j)
	for (i=0 ; i < net->n ; i++) {
		for (j=0 ; j < net->k ; j++)
			compute_weight (net, i, j);
	}
	
	net->w_init = 1;
	
	debug ("%s","\nWeights list:\n");
	dfor (i=0 ; i < net->n ; i++) {
		debug ("%ld)\t", i);
		dfor (j=0 ; j < net->k ; j++)
				debug ("%ld\t", net->w[j+i*net->k]);
		debug ("%s","\n");
	}
	
	return;
}


/* Initializes the state of the network over the stored memory XI[nu]
 *
 * PRE: net != NULL
 *	sdhn_init_XI  was previously used on net
 *	0 <= nu <= # of memories in net
 *
 * POS: S == XI[nu]
 */
void
sdhn_init_S (sdhn_t net, unsigned int nu)
{
	int i = 0, N = 0;
	
	assert (net != NULL);
	assert (net->XI_init);
	assert (nu < net->p);
	
	N = net->n/MSB + !(!(net->n % MSB));
	for (i=0 ; i < N ; i++)
		net->S[i] = net->XI[i+nu*N];
	
	debug ("\nState (memory #%u):\n", nu);
	dfor (i=0 ; i<N ; i++)
		printbits(net->S[i]);
	debug ("%s","\n");
	
	return;
}



/** ### ### ### ~~~~~~~~~ NETWORK DINAMIC FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~ */


/* Updates the whole network in a single time step, using DETERMINISTIC logic */
static void
update_net (sdhn_t net)
{
	int i = 0, j = 0;
	long N = 0, h = 0, ne = 0, Sn = 0;
	unsigned long mask = 0;
	unsigned long *aux = NULL;
	
	N = net->n/MSB + !(!(net->n % MSB));
	
	#pragma omp parallel for default(shared) private(i,j,h,ne,mask,Sn)
	for (i=0 ; i<N ; i++) {
		
		h = 0;
		for (j=0 ; j < net->k ; j++) {
			
			/* Sn = S[neigh[i][j]] */
			ne = net->neigh[j+i*net->k];
			mask = ((unsigned long) 1) << (ne%MSB);
			Sn = (net->S[ne/MSB] & mask) > 0 ? 1 : -1;
			
			/* h += w[i][j] * Sn   */
			h += net->w[j+i*net->k] * Sn;
		}
		
		mask = ((unsigned long) 1) << (i%MSB);
		/* If h >= 0 then mask holds the position of the bit  *
		 * we have to set (to 1) in S[i/MSB]		      *
		 * Otherwise mask's bitwise negation has a single '0' *
		 * in the exact position of the bit we have to reset  */
		/** WARNING is this thread-safe ??? */
		if (h > 0)
			net->SD[i/MSB] |= mask;
		else
			net->SD[i/MSB] &= ~mask;
	}
	
	/* State update */
	aux = net->S;
	net->S = net->SD;
	net->SD = aux;
	
	return;
}



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
	long N = 0, t = 0, i = 0, b = 0;
	double overlap = 0.0, norm = 0.0;
	
	assert (net != NULL);
	assert (net->w_init);
	assert (nu < net->p);
	
	N = net->n/MSB + !(!(net->n % MSB));
	norm = 1.0 / ((double) net->n);
	
	/* Relaxation */
	for (t=0 ; t < net->untraced ; t++) {
		update_net (net);
	}
	
	/* Measurement */
	for (t=0 ; t < net->traced ; t++) {
		
		update_net (net);
		
		/* b = XOR (S, ~XI[nu]) */
		b = 0;
		for (i=0 ; i<N ; i++)
			b += bitcount (net->S[i] ^ ~(net->XI[nu*N+i]));
		
		overlap += 2.0*b - ((double)net->n);
		
		debug ("Update #%ld\tOverlap = %.4f\n",
		       t, (2.0*b - ((double)net->n))*norm);
	}
	debug ("\nFinal overlap = %.8f\n", (overlap*norm) / ((double) net->n));
	
	return (overlap*norm) / ((double) net->traced);
}

