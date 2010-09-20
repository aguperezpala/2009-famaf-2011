#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <omp.h>
#include "mzran13.h"


#define  _byte_size  (1<<3)
#define  LSB  0
#define  MSB  (_byte_size*sizeof(unsigned long))
#define  MAX_ITER  30


/* # of arguments the main function should receive as input */
#define  ARGC  3
/* These are:	1) N ...... # of neurons in the net
 *		2) Pmax ... max # of stored memories
 *		3) hop .... by how much the # of memories in the net has to
 *			    increase at each step till Pmax is reached
 */


/* Global vars. See predefined constant ARGC */
unsigned long	N = 0,		/* # of neurons in the net */
		P = 0,		/* # of memories in the net */
		Pmax = 0,	/* max # of memories in the net */
		hop = 0;	/* magnitude of P hopping till Pmax reached */


/* Gives position (i,j) of a matrix[P][N]
 * Var names P,N should be within scope where called */
#define  AT(i,j)  ((i)*P+((j)%N))



/* Counts set bits in x (ie: bits in x equal to '1') */
static inline int
bitcount (unsigned long x)
{
	int b = 0;
	for (b = 0; x != 0; x &= (x-1))
		b++;
	return b;
}



/* Not much to specify about this function */
static void
parse_input (int argc, char **argv,
	     unsigned long *N, unsigned long *Pmax, unsigned long *hop)
{
	char *err = NULL;
	
	/* Checking # of arguments is right */
	if (argc != ARGC+1) {
		fprintf (stderr, "\nEl programa debe invocarse con 3 entradas:"
				 "\n1) # de neuronas de la red"
				 "\n2) Capacidad maxima de memorias a almacenar"
				 "\n3) El paso: de a cuanto debe incrementarse "
				 "el # de memorias del sistema cada vez\n\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*N = (unsigned long) strtol (argv[1], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				 "Debe pasar el # de neuronas de la red "
				 "como primer argumento\n", err);
		exit (EXIT_FAILURE);
	} else if (*N % MSB != 0) {
		fprintf (stderr, "Lo siento, pero el # de memorias de la red "
				 "debe ser un multiplo de %lu por cuestiones "
				 "implementativas\nIntentelo nevamente\n", MSB);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving max # of memories */
	*Pmax = (unsigned long) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				 "Debe pasar el maximo # de memorias de la red "
				 "como segundo argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving hop magnitude */
	*hop = (unsigned long) strtol (argv[3], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				 "Debe pasar el salto (de incremento de "
				 "memorias) como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	return;
}



/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~ ### Private functinos for bitwise structure handling ### ~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Initilizes randomly all p memories in XI[p][n] matrix
 * Each bit of each position in XI is considered a ξ[supra-μ][sub-i] component
 * PRE: XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 */
static void
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
	#pragma omp parallel for shared(XIbit)
	for (i=0 ; i<p*n ; i++) {
		XI[i] = (unsigned long) mzran13();
	}
	
	return;
}



/* Initializes the state 'S' of the neural network in a position randomly
 * close to the stored memory XI[nu]
 * Memory is handled bitxbit. ie: there are 64 components in each XI[mu][i]
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	S[i] == 0  ∀ i ∈ {0,1,...,n-1}
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	nu < p
 */
static void
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
			if (mzran13() % 2)
				/* tmp = ξ[nu][j] */
				tmp = ((unsigned long)1) << (MOD-1-j);
			else
				/* tmp = ξ[nu][1] */
				tmp = ((unsigned long)1) << (MOD-1);
			tmp &= XI[base+i];
			S[i] |= tmp;
		}
	}
	
	return;
}



/* Jumps one time-step ahead in time, for all the neurons in the neural network
 * This is done updating each neuron stored in 'S', and registering in 'm' all 
 * the new overlaps between 'S' and each 'XI[mu]'
 *
 * Each element is considered to occupy one bit of memory space. ie: for each
 * S[i] or XI[mu][i] position there are 64 stored components
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 */
static void
update_network_one_step (unsigned long *S, unsigned long *XI, int *m,
			 unsigned int n, unsigned int p)
{
	int i = 0, j = 0, mu = 0;
	unsigned long oldSi = -1, XImui = 0, mask = 0;
	long b = 0;
	double h = 0.0;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	
	/* Updating each neuron. With 'i' we step on a position within S,
	 * and with 'j' we access (one bit at a time) all 'MSB' neurons stored
	 * there. Usually MSB == 64
	 */
	for (i=0 ; i<n ; i++) {
		
		for (j=LSB ; j<MSB ; j++) {
			
			mask = ((unsigned long) 1) << j;
			h = 0.0;
			
			oldSi = S[i] & mask;
			#pragma omp parallel for shared(XIbit,m) private(XImui) \
								 reduction(+:h)
			for (mu=0 ; mu<p ; mu++) {
				XImui = XI[(mu*n)+i] & mask;
				h += (double) (norm(XImui) * m[mu]);
			}
			/* If h >= 0 then mask holds the position of the bit
			 * we have to set (to 1) in S[i]
			 * Otherwise mask's bitwise negation has a single '0'
			 * in the exact position of the bit we have to reset
			 */
			if (h >= 0)
				S[i] |= mask;
			else
				S[i] &= ~mask;
		}
	}
	/* Updating overlaps in m */
	#pragma omp parallel for default(shared) private(mu,b,i)
	for (mu=0 ; mu<p ; mu++) {
		long nn = n & ( ~(((unsigned long) 1) << (MOD-1)));
		b = 0;
		/* Negative logic is used on XI before XOR'ing it with S */
		for (i=0 ; i<n ; i++)
			b += bitcount((unsigned long) (~XI[(mu*n)+i]) ^ S[i]);
		m[mu] = 2*b - nn;
	}
	
	return;
}



/* Updates the neural network until a fixed point is reached for 'S'
 * The final overlap between 'S' and XI[nu] is the returned value 
 *
 * PRE: S != NULL
 *	S is an [n] dimensional vector
 *	XI != NULL
 *	XI is a [p][n] matrix (ie: holding 'p' memories of 'n' components each)
 *	m != NULL
 *	m is a [p] dimensional vector
 *	nu < p
 */
static long
run_network (unsigned long *S, unsigned long *XI, int *m,
	     unsigned int n, unsigned int p, unsigned int nu)
{
	long overlap = 0, mu = 0, i = 0, Sold = 0;
	unsigned long *Sold = NULL;
	bool fixed = false, differ = false;
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	assert (nu < p);
	
	Sold = (unsigned long *) calloc (n, sizeof(unsigned long));
	assert (Sold != NULL);
	
	/* Sold = S */
	Sold = memcpy (Sold, (const void *) S, (size_t) n*sizeof(unsigned long));
	
	while (!fixed) {
		update_network_one_step (S, XI, m, n, p);
		i = 0;
		while (!differ && i < n) {
			differ = Sold[i]-S[i];
			i++
		}
		if (differ)
			Sold = memcpy (Sold, (const void *) S,
				       (size_t) n*sizeof(unsigned long));
		else
			fixed = true;
	}
	
	free (Sold);	Sold = NULL;
	
	return overlap;
}





/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~ ### MAIN PROGRAM ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


int main (int argc, char **argv)
{
	/* We have access to the following global vars:
	unsigned long	N = 0,		# of neurons in the net
			P = 0,		# of memories in the net
			Pmax = 0,	max # of memories in the net
			hop = 0;	magnitude of P hopping till Pmax reached
	*/
	unsigned int NN = 0;
	unsigned long	*S = NULL,	/* Network state (N vector) */
			*XI=NULL;	/* Stored memories (PxN matrix) */
	int *m = NULL;			/* S-XI overlaps (P vector) */
	long i = 0, j = 0, k = 0
	unsigned int nu = 0;
	double start = 0.0, end = 0.0;
	
	
	parse_input (argc, argv, &NN, &Pmax, &hop);
	printf ("Argumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t\t\t%lu\n"
		"\tb) Máximo # de memorias de la red:\t\t%lu\n"
		"\tc) Magnitud del salto en el # de memorias:\t%lu\n",
		NN, Pmax, hop);
	
	/* Remember work will be bitwise */
	N = NN / MSB;
	
	/* Generating arrays */
	S = (unsigned long *) calloc (N, sizeof(unsigned long));
	assert (S != NULL);
	
	XI = (unsigned long *) calloc (N*Pmax, sizeof(unsigned long));
	assert (XI != NULL);
	
	m = (int *) calloc (Pmax, sizeof(int));
	assert (m != NULL);
	
	/* :p */
	for (P=hop, nu=0 ; P <= (long) Pmax/hop ; P += hop) {
		if (nu==0) {
			init_XI (XI, P, N);
			nu = P-1;
		} else
			nu--;
		
		/* We start somewhere close to XI[nu] */
		init_S (S, N, XI, P, nu);
			
		for ()
	}
	
	
	free (S);	S = NULL;
	free (XI);	XI = NULL;
	free (m);	m = NULL;
	
	return EXIT_SUCCESS;
}
