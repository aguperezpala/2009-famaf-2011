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

#define  _byte_size  (1<<3)
#define  LSB  0
#define  MSB  (_byte_size*sizeof(unsigned long))
#define  MAX_ITER  30
/** NOTE Uncomment the following definition for pretty output printing */
#define  PP


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
/** ~~~~~~~~~~~~~~~~~~~~~~~ ### MAIN PROGRAM ### ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


int main (int argc, char **argv)
{
/*	We have access to the following global vars:
	unsigned long	N = 0,		# of neurons in the net
			P = 0,		# of memories in the net
			Pmax = 0,	max # of memories in the net
			hop = 0;	magnitude of P hopping till Pmax reached
*/	unsigned long NN = 0;
	unsigned long	*S = NULL,	/* Network state (N vector) */
			*XI=NULL;	/* Stored memories (PxN matrix) */
	long *m = NULL;			/* S-XI overlaps (P vector) */
	long k = 0, overlap = 0;
	unsigned int nu = 0;
	double norm = 0.0;
	
	parse_input (argc, argv, &NN, &Pmax, &hop);
#ifdef PP
	printf ("\nVersion DETERMINISTICA del modelo de HOPFIELD para "
		"redes neuronales\n\nArgumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t\t\t%lu\n"
		"\tb) Máximo # de memorias de la red:\t\t%lu\n"
		"\tc) Magnitud del salto en el # de memorias:\t%lu\n",
		NN, Pmax, hop);
#endif
	
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
	
#ifdef PP
	printf ("\n____________________________________________________________"
		"_____\n|       α\t|\t     μ\t\t|\t    σ²\t\t|\n|~~~~~~~~~"
		"~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|\n");
#endif		
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
		nu = 0;
		init_XI (XI, P, N);
		
		for (k=1 ; k <= MAX_ITER ; k++) {
			
			/* We start at XI[nu] ... */
			init_S (S, N, XI, P, nu);
			update_overlaps (S, XI, m, N, P);
			
			/* ... and make the network update itself until a fixed
			 * point is reached */
			overlap = run_det_network (S, XI, m, N, P, nu);
			
			media_m ((double) overlap * norm, (double) k);
			var_m ((double) overlap * norm, (double) k);
			
			/* We recalculate our stored memories only if needed */
			if (nu < P-1) {
				nu++;
			} else {
				init_XI (XI, P, N);
				nu = 0;
			}
		}
#ifdef PP
		printf ("|  %.8f\t|\t%.8f\t|\t%.8f\t|\n",
			(double)P * norm, get_media_m(), get_var_m());
#else
		printf ("%.8f\t%.8f\t%.8f\t\n",
			(double)P * norm, get_media_m(), get_var_m());
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
