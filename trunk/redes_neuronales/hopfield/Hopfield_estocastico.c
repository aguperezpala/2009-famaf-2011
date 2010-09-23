#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "mzran13.h"
#include "ssv.h"
#include "ndf.h"




/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~ ### CONSTANTS & GLOBAL VARS ### ~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define  _byte_size	(1<<3)
#define  LSB		0
#define  MSB		(_byte_size*sizeof(unsigned long))

#define  MAX_ITER	30
#define  UNTRACED	100
#define  TRACED		40

/** NOTE Uncomment the following definition for pretty output printing */
/*#define  PP
*/

/* # of arguments the main function should receive as input */
#define  ARGC  4
/* These are:	1) N ...... # of neurons in the net (% MSB)
 *		2) P ...... # of memories in the net
 *		3) Tmax ... maximum noise level
 *		4) Thop ... by how much the net's noise level must increase
 *			    at each step till Tmax is reached
 */


/* Global vars. See predefined constant ARGC */
unsigned long	N = 0,	/* # of neurons in the net (% MSB) */
		P = 0;	/* # of memories in the net */
double	Tmax = 0,	/* max noise level */
	Thop = 0;	/* noise level increment per step until Tmax reached */


/* Counts set bits in x (ie: bits in x equal to '1') */
static inline int
bitcount (unsigned long x)
{
	int b = 0;
	for (b = 0; x != 0; x &= (x-1))
		b++;
	return b;
}



/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~ ### AUXILIARY FUNCTIONS ### ~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Not much to specify about this function */
static void
parse_input (int argc, char **argv,
	     unsigned long *N, unsigned long *P, double *Tmax, double *Thop)
{
	char *err = NULL;
	
	/* Checking # of arguments is right */
	if (argc != ARGC+1) {
		fprintf (stderr, "\nEl programa debe invocarse con 4 entradas:"
				 "\n1) # de neuronas de la red"
				 "\n2) # de memorias de la red"
				 "\n3) El máximo nivel de ruido"
				 "\n4) La magnitud de cada incremento en nivel "
				 "de ruido hasta alcanzar ese máximo\n\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*N = (unsigned long) strtol (argv[1], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de neuronas de la red "
				 "como primer argumento\n", err);
		exit (EXIT_FAILURE);
	} else if (*N % MSB != 0) {
		fprintf (stderr, "Lo siento, pero el # de memorias de la red "
				 "debe ser un multiplo de %lu por cuestiones "
				 "implementativas\nIntentelo nevamente\n", MSB);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of memories */
	*P = (unsigned long) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de memorias de la red "
				 "como segundo argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving Tmax magnitude */
	*Tmax = (double) strtod (argv[3], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el máximo nivel de ruido "
				 "como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	} else if (*Tmax <= 0.0) {
		fprintf (stderr, "%f es inválido como tercer argumento.\n"
				 "El máximo nivel de ruido debe ser "
				 "un # real positivo\n", *Tmax);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving Thop magnitude */
	*Thop = (double) strtod (argv[4], &err);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar la magnitud de los incrementos "
				 "graduales en el nivel de ruido\n"
				 "como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	return;
}





/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~ ### MAIN PROGRAM ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


int main (int argc, char **argv)
{
/*	We have access to the following global vars:
	unsigned long	N = 0,		# of neurons in the net
			P = 0,		# of memories in the net
			Pmax = 0,	max # of memories in the net
			Phop = 0;	magnitude of P Phopping till Pmax reached
*/	unsigned long NN = 0;
	unsigned long	*S = NULL,	/* Network state (N vector) */
			*XI=NULL;	/* Stored memories (PxN matrix) */
	long *m = NULL;			/* S-XI overlaps (P vector) */
	unsigned int nu = 0;
	double	T  = 0.0,		/* Noise level of the network */
		mt = 0.0,		/* Overlap (S ~ XI[nu]) for a fixed T */
		norm = 0.0;
	long k = 0;
	
	parse_input (argc, argv, &NN, &P, &Tmax, &Thop);
#ifdef PP
	printf ("\nVersion ESTOCÁSTICA del modelo de HOPFIELD para "
		"redes neuronales\n\nArgumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t%lu\n"
		"\tb) # de memorias de la red:\t%lu\n"
		"\tc) Máximo nivel de ruido:\t%f\n"
		"\td) Saltos en el nivel de ruido:\t%f\n",
		NN, P, Tmax, Thop);
#endif
	
	/* Remember work will be bitwise */
	N = NN / MSB;
	norm = 1.0 / (double) NN;
	
	/* Generating arrays */
	S = (unsigned long *) calloc (N, sizeof(unsigned long));
	assert (S != NULL);
	
	XI = (unsigned long *) calloc (N*P, sizeof(unsigned long));
	assert (XI != NULL);
	
	m = (long *) calloc (P, sizeof(long));
	assert (m != NULL);
	
#ifdef PP
	printf ("\n____________________________________________________________"
		"_____\n|       T\t|\t     μ\t\t|\t    σ²\t\t|\n|~~~~~~~~~"
		"~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|\n");
#endif		
	/* We start our network with a noise level T = Thop, and keep adding
	 * Thop to T at each step until Tmax is reached.
	 *
	 * For each T the network is set to work for 'UNTRACED' time units
	 * without any kind of measurement on its whereabouts.
	 *
	 * After that it is updated for 'TRACED' time units, asking each time
	 * the state's (S) distance from a certain memory XI[nu]. The TRACED
	 * values gathered are then averaged into a single value ('mt')
	 *
	 * This algorithm is performed MAX_ITER times for each T, and the
	 * resulting mean value for 'mt' toghether with its variance is printed
	 * through stdout.
	 */
	set_stoc_network (UNTRACED, TRACED);
	
	for (T = Thop ; T <= Tmax ; T += Thop) {
		
		reset_media_m ();
		reset_var_m ();
		nu = 0;
		init_XI (XI, P, N);
		
		for (k=1 ; k <= MAX_ITER ; k++) {
			
			/* We start at XI[nu] ... */
			init_S (S, N, XI, P, nu);
			update_overlaps (S, XI, m, N, P);
			
			/* ... and make the network update itself until a fixed
			 * point is reached */
			mt = run_stoc_network (S, XI, m, N, P, nu, T);
			
			media_m (mt * norm, (double) k);
			var_m (mt * norm, (double) k);
			
			/* We recalculate our stored memories only if needed */
			if (nu < P-1)
				nu++;
			else {
				init_XI (XI, P, N);
				nu = 0;
			}
		}
#ifdef PP
		printf ("|  %.8f\t|\t%.8f\t|\t%.8f\t|\n",
			T, get_media_m(), get_var_m());
#else
		printf ("%.8f\t%.8f\t%.8f\t\n",
			T, get_media_m(), get_var_m());
#endif
	}
#ifdef PP
	printf ("============================================================"
		"=====\nFin del programa\n\n");
#endif
	
	free (S);	S = NULL;
	free (XI);	XI = NULL;
	free (m);	m = NULL;
	
	return EXIT_SUCCESS;
}
