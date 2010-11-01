/*
 * Neural Networks - Deterministic Hopfield Model with Strong Dilution
 * Network ADT
 *
 * $ Author: Carlos E. Budde $
 * $ Date: 31/10/2010 21:32 $
 * $ License: GPL v3 $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "sdhn.h"
#include "ssv.h"


/* NOTE Uncomment the following definition for pretty output printing */
// #define  PP



/* # of arguments the main function should receive as input */
#define  ARGC  4
/* These are:	1) N ...... # of neurons in the net
 *		2) C ...... # of neighbours per neuron ("mean conectivity")
 *		3) Pmax ... max # of stored memories
 *		4) Phop ... by how much the # of memories in the net has to
 *			    increase at each step till Pmax is reached
 */


#define  TRACED   100	/* # of steps the network will run measuring overlaps */
#define  UNTRACED  30	/* # of steps the network will run "silently" */
#define  KMAX      50	/* # of repetitions per measurement */



/* Not much to specify about this function */
static void
parse_input (int argc, char **argv,
	     unsigned int *N,    unsigned int *C,
	     unsigned int *Pmax, unsigned int *Phop)
{
	char *err = NULL;
	
	/* Checking # of arguments is right */
	if (argc != ARGC+1) {
		fprintf (stderr, "\nEl programa debe invocarse con 4 entradas:"
				 "\n 1) # de neuronas de la red"
				 "\n 2) # de vecinos por cada neurona"
				 "\n 3) máximo # de memorias de la red"
				 "\n 4) tamaño del salto en el # de memorias\n\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*N = (unsigned int) strtol (argv[1], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de neuronas de la red "
				 "como primer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving mean conectivity */
	*C = (unsigned int) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de vecinos por neurona "
				 "como segundo argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving max # of memories */
	*Pmax = (unsigned int) strtol (argv[3], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el máximo # de memorias de la "
				 "red como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving Phop */
	*Phop = (unsigned int) strtol (argv[4], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el salto (de incremento de "
				 "memorias) como cuarto argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	return;
}




int main (int argc, char **argv)
{
	sdhn_t net = NULL;	/* Neural Network */
	unsigned int N = 0,	/* # of neurons in the net */
		     C = 0,	/* # of neighbours per neuron */
		     Pmax = 0,	/* max # of memories to store */
		     Phop = 0;	/* step increment in the # of memories  */
	int nu = 0,	/* current memory to overlap with */
	    p  = 0,	/* current # of memories in the net */
	    k  = 0;
	
	double overlap = 0.0;
	
	parse_input (argc, argv, &N, &C, &Pmax, &Phop);
	
#ifdef PP
	printf ("\nVersion ULTRA DILUIDA del modelo de HOPFIELD para "
		"redes neuronales\n\nArgumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t\t\t%u\n"
		"\tb) # de vecinos por neurona:\t\t\t%u\n"
		"\tc) Máximo # de memorias de la red:\t\t%u\n"
		"\td) Magnitud del salto en el # de memorias:\t%u\n",
		N, C, Pmax, Phop);
	
	printf ("\n____________________________________________________________"
		"_____\n|       α'\t|\t     μ\t\t|\t    σ²\t\t|\n|~~~~~~~~~"
		"~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|\n");
#endif
	
	for (p = Phop ; p <= Pmax ; p += Phop) {
		
		/* Create new network with 'p' memories */
		net = sdhn_create (N, p, C);
		sdhn_set_traced   (net, TRACED);
		sdhn_set_untraced (net, UNTRACED);
		
		/* Restart statistical values */
		reset_media_m ();
		reset_var_m ();
		nu = 0;
		
		for (k=0 ; k<KMAX ; k++) {
			
			/* Choose a new memory to overlap with */
			if (--nu < 0) {
				sdhn_init_XI (net);
				nu = p-1;
				/* NOTE
				 * ¿Estas 2 actualizaciones pueden ir acá?
				 * ¿O hay que hacerlas en todas las vueltas? */
				sdhn_init_neigh (net);
				sdhn_init_w (net);
			}
			
			sdhn_init_S (net, nu);
			
			/* Run the network for TRACED time intervals */
			overlap = sdhn_run_net (net, nu);
			
			media_m (overlap , k+1);
			var_m (overlap , k+1);
		}
		
		net = sdhn_destroy (net);
#ifdef PP
		printf ("|  %.8f\t|\t%.8f\t|\t%.8f\t|\n",
			((double)p) / ((double)C), get_media_m(), get_var_m());
#else
		printf ("%.8f\t%.8f\t%.8f\t\n",
			((double)p) / ((double)C), get_media_m(), get_var_m());
#endif
	}
#ifdef PP
	printf ("============================================================"
		"=====\nFin del programa\n\n");
#endif
	
	return 0;
}
