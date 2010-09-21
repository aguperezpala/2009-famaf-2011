#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h> /* To have access to uuint64_t */
#include "mzran13.h"
#include "ssv.h"


#define  _byte_size  (1<<3)
#define  LSB  0
#define  MSB  (_byte_size*sizeof(unsigned long))
#define  MAX_ITER  30


/* # of arguments the main function should receive as input */
#define  ARGC  3
/* These are:	1) N ...... # of neurons in the net (% MSB)
 *		2) Pmax ... max # of stored memories
 *		3) hop .... by how much the # of memories in the net has to
 *			    increase at each step till Pmax is reached
 */


/* Global vars. See predefined constant ARGC */
unsigned long	N = 0,		/* # of neurons in the net (% MSB) */
		P = 0,		/* # of memories in the net */
		Pmax = 0,	/* max # of memories in the net */
		hop = 0;	/* magnitude of P hopping till Pmax reached */


/* Normalización a (-1,+1) */
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
 *
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
	#pragma omp parallel for shared(XI)
	for (i=0 ; i<p*n ; i++) {
		XI[i] = (unsigned long) mzran13();
	}
	
	return;
}



/* Initializes the state 'S' of the neural network in a position randomly
 * close to the stored memory XI[nu]
 * Memory is handled bitxbit. ie: there are 64 components in each XI[mu][i]
 *
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
static void
update_overlaps (unsigned long *S, unsigned long *XI, long *m,
		 unsigned int n, unsigned int p)
{
	int mu = 0, i = 0;
	long b = 0, nn = n & (~(((unsigned long) 1) << (MSB-1)));
	
	assert (S  != NULL);
	assert (XI != NULL);
	assert (m  != NULL);
	
	#pragma omp parallel for default(shared) private(mu,b,i)
	for (mu=0 ; mu<p ; mu++) {
		b = 0;
		/* Negative logic is used on XI before XOR'ing it with S */
		for (i=0 ; i<n ; i++)
			b += bitcount((unsigned long) (~XI[(mu*n)+i]) ^ S[i]);
		m[mu] = 2*b - (nn*MSB);
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
update_network_one_step (unsigned long *S, unsigned long *XI, long *m,
			 unsigned int n, unsigned int p)
{
	int i = 0, j = 0, mu = 0;
	unsigned long oldSi = 0, XImui = 0, mask = 0;
	double h = 0.0;
	
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
			
			#pragma omp parallel for shared(XI,m) private(XImui) \
								reduction(+:h)
			for (mu=0 ; mu<p ; mu++) {
				XImui = XI[(mu*n)+i] & mask;
				h += (double) (norm(XImui) * m[mu]);
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
			if (S[i] != oldSi)
				update_overlaps(S, XI, m, n, p);
		}
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
run_network (unsigned long *S, unsigned long *XI, long *m,
	     unsigned int n, unsigned int p, unsigned int nu)
{
	long overlap = 0, i = 0, nn = n & (~(((unsigned long) 1) << (MSB-1)));
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
		while (!S_changed && i < n) {
			S_changed = Sold[i]-S[i] != 0;
			i++;
		}
		if (S_changed)
			Sold = copyvector(Sold,S,n);
		
		niter++;
	}
	
	overlap = 0;
	/* Negative logic is used on XI before XOR'ing it with S */
	#pragma omp parallel for shared(XI,S) reduction(+:overlap)
	for (i=0 ; i<n ; i++)
		overlap += bitcount((unsigned long) (~XI[(nu*n)+i]) ^ S[i]);
	
	free (Sold);	Sold = NULL;
	
	return 2*overlap - (nn*MSB);
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
	unsigned long NN = 0;
	unsigned long	*S = NULL,	/* Network state (N vector) */
			*XI=NULL;	/* Stored memories (PxN matrix) */
	long *m = NULL;			/* S-XI overlaps (P vector) */
	long k = 0, overlap = 0;
	unsigned int nu = 0;
	double norm = 0.0;
	
	
	parse_input (argc, argv, &NN, &Pmax, &hop);
	printf ("\nVersion DETERMINISTICA del modelo de HOPFIELD para "
		"redes neuronales\n\nArgumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t\t\t%lu\n"
		"\tb) Máximo # de memorias de la red:\t\t%lu\n"
		"\tc) Magnitud del salto en el # de memorias:\t%lu\n",
		NN, Pmax, hop);
	
	/* Remember work will be bitwise */
	N = NN / MSB;
	norm = 1.0 / (double) NN;
	
	/* Generating arrays */
	S = (unsigned long *) calloc (N, sizeof(unsigned long));
	assert (S != NULL);
	
	XI = (unsigned long *) calloc (N*Pmax, sizeof(unsigned long));
	assert (XI != NULL);
	
	m = (long *) calloc (Pmax, sizeof(long));
	assert (m != NULL);
	
	printf ("\n____________________________________________________________"
		"_____\n|       α\t|\t     μ\t\t|\t    σ²\t\t|\n|~~~~~~~~~"
		"~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|\n");
	
	/* We start our network with a # of memories P = hop, and add 'hop'
	 * memories at each step until Pmax is reached.
	 *
	 * For each P the network is set to work until the state 'S' reaches a
	 * fixed point. Then the distance from 'S' to a chosen memory XI[nu]
	 * is calculated and stored in 'overlap'
	 *
	 * This is done MAX_ITER times for each P, and the resulting mean value
	 * of 'overlap' toghether with its variance is printed through stdout.
	 */
	for (P=hop ; P <= (long) Pmax ; P += hop) {
		
		reset_media_m ();
		reset_var_m ();
		
		for (k=1, nu=0 ; k <= MAX_ITER ; k++) {
			
			/* We recalculate our stored memories if needed */
			if (nu-- <= 0) {
				init_XI (XI, P, N);
				nu = P-1;
			}
			
			/* We start somewhere close to XI[nu] ... */
			init_S (S, N, XI, P, nu);
			update_overlaps (S, XI, m, N, P);
			
/*			overlap = 0;
			#pragma omp parallel for shared(XI,S) reduction(+:overlap)
			for (int i=0 ; i<N ; i++)
				overlap += bitcount((unsigned long) (~XI[(nu*N)+i]) ^ S[i]);
			printf ("Initial overlap: %.4f\n",
				(2.0*(double)overlap - NN)*norm);
*/			
			/* ... and make the network update itself until a fixed
			 * point is reached */
			overlap = run_network (S, XI, m, N, P, nu);
			
			
/*			printf ("Final overlap: %.4f\n",
				(2.0*(double)overlap - NN)*norm);
*/				
			media_m ((double) overlap * norm, (double) k);
			var_m ((double) overlap * norm, (double) k);
		}
		
		printf ("|  %.8f\t|\t%.8f\t|\t%.8f\t|\n",
			(double)P * norm, get_media_m(), get_var_m());
	}
	printf ("============================================================"
		"=====\nFin del programa\n\n");
		
	free (S);	S = NULL;
	free (XI);	XI = NULL;
	free (m);	m = NULL;
	
	return EXIT_SUCCESS;
}
