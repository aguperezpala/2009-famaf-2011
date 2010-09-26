/*
 * Neural Networks - Deterministic Hopfield Model
 * Network dynamics' functions (NDFs)
 *
 * NOTE
 * All functions consider a BITWISE STORAGE within vectors and matrixes.
 * That is, each element is considered to occupy one bit of memory space.
 * ie: for each S[i] or XI[mu][i] position there are X stored components,
 * where X = sizeof(unsigned long). Usually X = 64.
 */

#ifndef NETWORK_DYNAMIC_FUNCTIONS_H
#define NETWORK_DYNAMIC_FUNCTIONS_H


/* Initilizes randomly all p memories in XI[p][n] matrix
 *
 * PRE: XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 */
void
init_XI (unsigned long *XI, unsigned int p, unsigned int n);



/* Initializes the state 'S' of the neural network in the stored memory XI[nu]
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
	unsigned long *XI, unsigned int p, unsigned int nu);



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
		 unsigned int n, unsigned int p);



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
		 unsigned int n, unsigned int p, unsigned int nu);



/* Sets new values for the UNTRACED/TRACED # of updates (or time periods)
 * for the next call to run_stoc_network
 *
 * PRE: tr > 0
 */
void
set_stoc_network (unsigned int untr, unsigned int tr);


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
double
run_stoc_network (unsigned long *S, unsigned long *XI, long *m,
		  unsigned int n, unsigned int p, unsigned int nu, double T);



#endif
