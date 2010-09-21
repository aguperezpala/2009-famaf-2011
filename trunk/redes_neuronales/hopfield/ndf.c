#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
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
/*	#pragma omp parallel for shared(XI)
*/	for (i=0 ; i<p*n ; i++) {
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
	
/*	#pragma omp parallel for default(shared) private(i,j,tmp)
*/	for (i=0 ; i<n ; i++) {
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
	
/*	#pragma omp parallel for default(shared) private(mu,b,i)
*/	for (mu=0 ; mu<p ; mu++) {
		b = 0;
		/* Negative logic is used on XI before XOR'ing it with S */
		for (i=0 ; i<n ; i++)
			b += bitcount ((unsigned long) (~XI[(mu*n)+i]) ^ S[i]);
		m[mu] = (2*b) - (nn*MSB);
	}
	
	return;
}



/* Jumps one time-step ahead in time, for all the neurons in the neural network
 * This is done updating each neuron stored in 'S', and registering in 'm' all
 * the new overlaps between 'S' and each 'XI[mu]'
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 */
void
update_network_one_step (unsigned long *S, unsigned long *XI, long *m,
			 unsigned int n, unsigned int p)
{
	int i = 0, j = 0, mu = 0;
	unsigned long	oldSi = 0,
			XImui = 0,
			mask = 0;
	double	h = 0.0,
		nn = 1.0/((double) (n*MSB)),
		overlap = 0.0;
	
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
			h = 0.0;
			
/*			#pragma omp parallel for shared(XI,m) \
					private(XImui,overlap) reduction(+:h)
*/			for (mu=0 ; mu<p ; mu++) {
				XImui = XI[(mu*n)+i] & mask;
				overlap = (double) m[mu] * nn;
				h += ((double) norm(XImui)) * overlap;
			}
			oldSi = S[i];
			/* If h >= 0 then mask holds the position of the bit
			 * we have to set (to 1) in S[i]
			 * Otherwise mask's bitwise negation has a single '0'
			 * in the exact position of the bit we have to reset
			 */
			if (h >= 0)
				S[i] |= mask;
			else
				S[i] &= ~mask;
			/* Recalculating overlaps in m (if needed) */
			if (S[i] != oldSi) {
				update_overlaps(S, XI, m, n, p);
			}
		}
	}
	
	return;
}



/* Updates the neural network until a fixed point is reached for 'S'
 * The final overlap between 'S' and XI[nu] is the returned value
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
run_network (unsigned long *S, unsigned long *XI, long *m,
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
	
	while (S_changed) {
			
		update_network_one_step (S, XI, m, n, p);
		
		i = 0;
		S_changed = false;
		while (!S_changed && (i < n)) {
			S_changed = Sold[i]-S[i] != 0;
			i++;
		}
		if (S_changed)
			Sold = copyvector(Sold,S,n);
		
		niter++;
	}
	
	free (Sold);	Sold = NULL;
	
	return m[nu];
}

