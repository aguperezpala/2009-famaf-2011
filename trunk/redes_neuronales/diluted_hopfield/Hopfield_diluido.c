#include <stdio.h>
#include "sdhn.h"
#include "ssv.h"


/* NOTE Uncomment the following definition for pretty output printing */
/*#define  PP
*/


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
#define  K  40		/* # of repetitions per measurement */



/* Not much to specify about this function */
static void
parse_input (int argc, char **argv,
	     unsigned long *N,    unsigned long *C,
	     unsigned long *Pmax, unsigned long *Phop)
{
	char *err = NULL;
	
	/* Checking # of arguments is right */
	if (argc != ARGC+1) {
		fprintf (stderr, "\nEl programa debe invocarse con 4 entradas:"
				 "\n1) # de neuronas de la red"
				 "\n2) # de vecinos por cada neurona"
				 "\n3) máximo # de memorias de la red"
				 "\n4) tamaño del salto en el # de memorias\n\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*N = (unsigned long) strtol (argv[1], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de neuronas de la red "
				 "como primer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving mean conectivity */
	*C = (unsigned long) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el # de vecinos por neurona "
				 "como segundo argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving max # of memories */
	*Pmax = (unsigned long) strtol (argv[3], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el máximo # de memorias de la "
				 "red como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving Phop */
	*Phop = (unsigned long) strtol (argv[4], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena '%s'\n"
				 "Debe pasar el salto (de incremento de "
				 "memorias) como cuarto argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	return;
}




int main (void)
{
	sdhn_t net = NULL;	/* Neural Network */
	unsigned int N = 0,	/* # of neurons in the net */
		     C = 0,	/* # of neighbours per neuron */
		     Pmax = 0,	/* max # of memories to store */
		     Phop = 0;	/* step increment in the # of memories  */
	int k = 0, p = 0;
	double overlap = 0.0;
	
	parse_input (argc, argv, &N, &C, &Pmax, &Phop);
	
#ifdef PP
	printf ("\nVersion ULTRA DILUIDA del modelo de HOPFIELD para "
		"redes neuronales\n\nArgumentos recibidos:\n"
		"\ta) # de neuronas de la red:\t\t%lu\n"
		"\tb) # de vecinos por neurona:\t\t%lu\n"
		"\tc) Máximo # de memorias de la red:\t\t%lu\n"
		"\td) Magnitud del salto en el # de memorias:\t%lu\n",
		N, C, Pmax, Phop);
	
	printf ("\n____________________________________________________________"
		"_____\n|       α'\t|\t     μ\t\t|\t    σ²\t\t|\n|~~~~~~~~~"
		"~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|~~~~~~~~~~~~~~~~~~~~~~~|\n");
#endif
	
	for (p = Phop ; p <= Pmax ; p += Phop) {
		
		net = sdhn_create (N, p, C);
		sdhn_set_traced   (net, TRACED);
		sdhn_set_untraced (net, UNTRACED);
		
		reset_media_m ();
		reset_var_m ();
		nu = 0;
		
		for (k=0 ; k<K ; k++) {
			/* TODO
			 *	0. Init XI if needed
			 *	1. Choose a particular nu to overlap with
			 *	2. Init state
			 *	3. Init neighbours
			 *	4. Init weights
			 *	5. Run network
			 *	6. Save overlap in mean and variance
			 */
		}
		/* TODO: save resulting overlap */
		
		net = sdhn_destroy (net);
	}
	
	return 0;
}
