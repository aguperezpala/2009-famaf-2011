#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include "mzran13.h"
#include "ndf.h"


/* NOTE
 * All functions consider a BITWISE STORAGE within vectors and matrixes.
 * That is, each element is considered to occupy one bit of memory space.
 * ie: for each S[i] or XI[mu][i] position there are X stored components,
 * where X = sizeof(unsigned long). Usually X = 64.
 */



#define  _byte_size  (1<<3)
#define  LSB  0
#define  MSB  (_byte_size*sizeof(unsigned long))

/* Values normalization to (-1,+1) */
#define  norm(x)  ((x) > 0 ? 1 : -1)

/* Copies the contents of the vector 'src' into the vector 'dest'
 * Both vectors must be 'n' dimensional, and store unsigned longs */
#define  copyvector(dest,src,n) \
	 memcpy((dest), (const void *)(src), (size_t)(n)*sizeof(unsigned long))


/* Debugging printing function */
#ifdef _DEBUG
  #define  debug(s,...)  printf(s, ##__VA_ARGS__)
#else
  #define  debug(s,...)
#endif


unsigned int UNTRACED = 0,
	     TRACED   = 0;


/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/
/** ~~~~~~~~~~~~~~~~~~~~ ### COMMON FUNCTIONS ### ~~~~~~~~~~~~~~~~~~~~~~~~~~ **/


/* Counts set bits in x (ie: bits in x equal to '1') */
static inline int
bitcount (unsigned long x)
{
	int b = 0;
	for (b = 0; x != 0; x &= (x-1))
		b++;
	return b;
}



/* Initilizes randomly all p memories in XI[p][n] matrix
 *
 * PRE: XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 */
void
init_XI (unsigned long *XI, unsigned int p, unsigned int n)
{
	int i = 0;
	
	assert (XI != NULL);
	
	/* NOTE With #pragma statement this section becomes non-deterministic,
	 * as the output memory XI[i] will depend on the execution order of the
	 * threads, which itself is non-deterministic.
	 *	To regain determinism in the creation of the XI matrix simply
	 * comment the #pragma statement
	 */
	#pragma omp parallel for shared(XI)
	for (i=0 ; i<p*n ; i++) {
		XI[i] = (unsigned long) mzran13();
	}
	
	return;
}



/* Initializes the state 'S' of the neural network in a position randomly
 * close to the stored memory XI[nu]
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	S[i] == 0 ∀ i ∈ {0,1,...,n-1}
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	nu < p
 */
void
init_S (unsigned long *S , unsigned int n,
	unsigned long *XI, unsigned int p, unsigned int nu)
{
	int i = 0, j = 0, base = (int) nu*n;
	unsigned long tmp = 0;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (nu < p);
	
	#pragma omp parallel for default(shared) private(i,j,tmp)
	for (i=0 ; i<n ; i++) {
		for (j=0 ; j<MSB ; j++) {
			if ((mzran13() % 5) % 2)
				/* tmp = ξ[nu][j] */
				tmp = ((unsigned long)1) << (MSB-1-j);
			else
				/* tmp = ξ[nu][1] */
				tmp = ((unsigned long)1) << (MSB-1);
			tmp &= XI[base+i];
			S[i] |= tmp;
		}
	}
	
	return;
}



/* Updates the overlaping between the state 'S' and the memories in 'XI'
 * These overlaps are left registered in 'm'
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 */
void
update_overlaps (unsigned long *S, unsigned long *XI, long *m,
		 unsigned int n, unsigned int p)
{
	int mu = 0, i = 0;
	long b = 0, nn = n & (~(((unsigned long) 1) << (MSB-1)));
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	
	debug ("%s","\n\t\t> Updating overlaps\n");
	
	#pragma omp parallel for default(shared) private(mu,b,i)
	for (mu=0 ; mu<p ; mu++) {
		
		debug ("\t\t> m[%d]: %.4f", mu, (double) m[mu] / ((double) n*MSB));
		
		b = 0;
		/* Negative logic is used on XI before XOR'ing it with S */
		for (i=0 ; i<n ; i++)
			b += bitcount ((unsigned long) (~XI[(mu*n)+i]) ^ S[i]);
		m[mu] = (2*b) - (nn*MSB);
		
		debug ("  --->  %.4f\n", (double) m[mu] / ((double) n*MSB));
	}
	
	return;
}




/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/
/** ~~~~~~~~~~~~~~~~~ ### DETERMINISTIC FUNCTIONS ### ~~~~~~~~~~~~~~~~~~~~~~ **/


/* Jumps one time-step ahead in time, for all the neurons in the neural network
 * This is done updating each neuron stored in 'S', and registering in 'm' all
 * the new overlaps between 'S' and each 'XI[mu]'
 *
 * It uses DETERMINISTIC logic
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 */
static void
update_deterministic_network (unsigned long *S, unsigned long *XI, long *m,
			      unsigned int n, unsigned int p)
{
	int i = 0, j = 0, mu = 0;
	unsigned long	oldSi = 0,
			XImui = 0,
			mask = 0;
	long h = 0;
	double hh = 0.0;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	
	/* Updating each neuron.
	 * With 'i' we step on a position within S, and with 'j' we access
	 * (one bit at a time) all 'MSB' neurons stored there.
	 * Usually MSB == 64
	 */
	for (i=0 ; i<n ; i++) {
		for (j=LSB ; j<MSB ; j++) {
			
			mask = ((unsigned long) 1) << j;
			h = 0;
			
			#pragma omp parallel for shared(XI,m) \
					private(XImui) reduction(+:h)
			for (mu=0 ; mu<p ; mu++) {
				XImui = XI[(mu*n)+i] & mask;
				h += norm(XImui) * m[mu];
			}
			oldSi = S[i];
			/* hh = S[i] * p/N */
			hh = ((double) p * norm(S[i]&mask)) / ((double)(n*MSB));
			
			/* If h >= 0 then mask holds the position of the bit
			 * we have to set (to 1) in S[i]
			 * Otherwise mask's bitwise negation has a single '0'
			 * in the exact position of the bit we have to reset
			 */
			if (((double) h) - hh >= 0.0) {
				S[i] |= mask;
			} else {
				S[i] &= ~mask;
			}
			
			/* Recalculating overlaps in m (if needed) */
			if (S[i] != oldSi) {
				
				debug ("\n\t> S[%d] = %lu ---> %lu", i, oldSi, S[i]);
				
				update_overlaps(S, XI, m, n, p);
			}
		}
	}
	return;
}



/* Updates the neural network until a fixed point is reached for 'S'
 * The final overlap between 'S' and XI[nu] is the returned value
 *
 * It uses DETERMINISTIC transitions
 *
 * NOTE: this overlap must be divided by N before being used, where N is the
 *	 number of neurons in the network. ie: N = n*sizeof(unsigned long)
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 *	nu < p
 */
long
run_det_network (unsigned long *S, unsigned long *XI, long *m,
		 unsigned int n, unsigned int p, unsigned int nu)
{
	long i = 0;
	unsigned long *Sold = NULL, niter = 0;
	bool S_changed = true;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	assert (nu < p);
	
	Sold = (unsigned long *) calloc (n, sizeof(unsigned long));
	assert (Sold != NULL);
	
	/* Sold = S */
	Sold = copyvector(Sold,S,n);
	
	debug ("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
		"Run network\n> Memories: %u\n> Initial overlap: m[%u] = %.4f\n",
	        p, nu, (double) m[nu] / ((double)n*MSB));
	
	while (S_changed) {
		
#ifdef _DEBUG
		debug ("%s","\n> Sold:");
		for (i=0 ; i<n ; i++)
			debug (" %lu", Sold[i]);
#endif
		
		update_deterministic_network (S, XI, m, n, p);
		
#ifdef _DEBUG
		debug ("%s","\n> Snew:");
		for (i=0 ; i<n ; i++)
			debug (" %lu", S[i]);
#endif
		
		i = 0;
		S_changed = false;
		while (!S_changed && (i < n)) {
			S_changed = Sold[i]-S[i] != 0;
			i++;
		}
		if (S_changed) {
			Sold = copyvector(Sold,S,n);
			debug ("%s","\n> S_changed !");
		}
		niter++;
	}
	
	debug ("\n\n> Final overlap: m[%u] = %.4f\t\tniter: %lu\n\n",
	       nu, (double) m[nu] / ((double) n*MSB), niter);
	free (Sold);	Sold = NULL;
	
	return m[nu];
}



/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/
/** ~~~~~~~~~~~~~~~~~~ ### STOCHASTIC FUNCTIONS ### ~~~~~~~~~~~~~~~~~~~~~~~~ **/



/* Sets new values for the UNTRACED/TRACED # of updates (or time periods)
 * for the next call to run_stoc_network
 *
 * PRE: tr > 0
 */
void
set_stoc_network (unsigned int untr, unsigned int tr)
{
	assert (tr > 0);
	
	UNTRACED = untr;
	TRACED   = tr;
	
	return;
}



/* Jumps one time-step ahead in time, for all the neurons in the neural network
 * This is done updating each neuron stored in 'S', and registering in 'm' all
 * the new overlaps between 'S' and each 'XI[mu]'
 *
 * It uses DETERMINISTIC logic
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 *	T >= 0.0
 */
static void
update_stochastic_network (unsigned long *S, unsigned long *XI, long *m,
			   unsigned int n, unsigned int p, double T)
{
	int i = 0, j = 0, mu = 0;
	unsigned long	oldSi = 0,
			XImui = 0,
			mask = 0;
	long h = 0;
	double b = 0.0, prob = 0.0, z = 0.0;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	assert (T >= 0.0);
	
	if (T != 0.0)
		b = 1.0/T;
	else {
		fprintf (stderr, "\nupdate_stochastic_network: WARNING: "
			"noise-level T = 0\nWill self destruct in");
		for (i=3 ; i<=0 ; i--)
			fprintf (stderr, " %d", i);
		fprintf (stderr, "\n\nKABOOOOOOOOMMMMMM !!!!!!!!!!!\n\n");
		assert (false);
	}
	
	/* Updating each neuron.
	 * With 'i' we step on a position within S, and with 'j' we access
	 * (one bit at a time) all 'MSB' neurons stored there.
	 * Usually MSB == 64
	 */
	for (i=0 ; i<n ; i++) {
		for (j=LSB ; j<MSB ; j++) {
			
			mask = ((unsigned long) 1) << j;
			h = 0;
			
			#pragma omp parallel for shared(XI,m) \
					private(XImui) reduction(+:h)
			for (mu=0 ; mu<p ; mu++) {
				XImui = XI[(mu*n)+i] & mask;
				h += norm(XImui) * m[mu];
			}
			oldSi = S[i];
			prob = (1.0 + tanh (b * (double) h)) * 0.5;
			z = (double) mzran13() / (double) ULONG_MAX;
			
			if (z < prob) {
				/* "Turn ON the neuron" */
				S[i] |= mask;
			} else {
				/* "Turn OFF the neuron" */
				S[i] &= ~mask;
			}
			
			/* Recalculating overlaps in m (if needed) */
			if (S[i] != oldSi) {
				
				debug ("\n\t> S[%d] = %lu ---> %lu", i, oldSi, S[i]);
				
				update_overlaps(S, XI, m, n, p);
			}
		}
	}
	return;
}



/* Updates the neural network until a fixed point is reached for 'S'
 * The final overlap between 'S' and XI[nu] is the returned value
 *
 * It uses STOCHASTIC transitions
 *
 * NOTE: this overlap must be divided by N before being used, where N is the
 *	 number of neurons in the network. ie: N = n*sizeof(unsigned long)
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 *	nu < p
 *	T >= 0.0
 */
long
run_stoc_network (unsigned long *S, unsigned long *XI, long *m,
		  unsigned int n, unsigned int p, unsigned int nu, double T)
{
	long i = 0;
	unsigned long *Sold = NULL, niter = 0;
	double mt = 0.0;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	assert (nu < p);
	assert (T >= 0.0);
	
	Sold = (unsigned long *) calloc (n, sizeof(unsigned long));
	assert (Sold != NULL);
	
	/* Sold = S */
	Sold = copyvector(Sold,S,n);
	
	debug ("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
	"Run network\n> Memories: %u\n> Initial overlap: m[%u] = %.4f\n",
	p, nu, (double) m[nu] / ((double)n*MSB));
	
	for (i=0 ; i < UNTRACED ; i++) {
		update_stochastic_network (S, XI, m, n, p, T);
	}
	
	for (i=0 ; i < TRACED ; i++) {
		
#ifdef _DEBUG
		debug ("%s","\n> Sold:");
		for (int j=0 ; j<n ; j++)
			debug (" %lu", Sold[j]);
#endif
		update_stochastic_network (S, XI, m, n, p, T);
		mt += m[nu];
#ifdef _DEBUG
		debug ("%s","\n> Snew:");
		for (j=0 ; j<n ; j++)
			debug (" %lu", S[j]);
#endif
		niter++;
	}
	
	debug ("\n\n> Final overlap: m[%u] = %.4f\t\tniter: %lu\n\n",
	nu, (double) m[nu] / ((double) n*MSB), niter);
	free (Sold);	Sold = NULL;
	
	return mt / (double) TRACED;
}

