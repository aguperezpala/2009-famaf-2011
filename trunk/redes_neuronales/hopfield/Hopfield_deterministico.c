#include <stdio.h>
#include <stdlib.h>
#include "mzran13.h"


#define  _byte_size  (1<<3)
#define  SIZE  (_byte_size*sizeof(size_t))



/* # of arguments the main functino should receive as input */
#define  ARGC  3
/* These are:	1) N ...... # of neurons in the net
 *		2) Pmax ... max # of stored memories
 *		3) hop .... by how much the # of memories in the net has to
 *			    increase at each step till Pmax is reached
 */


/* Global vars. See arg */
unsigned long	N = 0,		/* # of neurons in the net */
		P = 0,		/* # of memories in the net */
		Pmax = 0,	/* max # of memories in the net*/
		hop = 0;	/* magnitude of P hopping till Pmax reached */


/* Gives position (i,j) of a matrix[P][N]
 * Var names P,N should be within scope where called */
#define  AT(i,j)  ((i)*P+((j)%N))



/* Not much to specify about this function */
static void parse_input (int argc, char **argv,
			 unsigned long *N, unsigned long *Pmax, unsigned long *hop)
{
	char *err = NULL;
	int size = SIZE;
	
	/* Checking # of arguments is right */
	if (argc != ARGC+1) {
		fprintf (stderr, "El programa debe invocarse con 3 entradas:\n"
				"1) El # de neuronas de la red\n
				"2) La capacidad maxima de memorias a almacenar\n"
				"3) El paso: de a cuanto debe incrementarse el "
				"# de memorias del sistema a cada paso\n");
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*N = (unsigned long) strtol (argv[1], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				"Debe pasar el # de neuronas de la red "
				"como primer argumento\n", err);
		exit (EXIT_FAILURE);
	} else if (*N % SIZE != 0) {
		fprintf (stderr, "Lo siento, pero el # de memorias de la red "
				"debe ser un multiplo de %d por cuestiones "
				"implementativas\nIntentelo nevamente\n", size);
		exit (EXIT_FAILURE);
	}
	
	if (*N > NMAX) {
		printf ("Truncating specified # of division intervals (%lu)\n"
		"to maximum allowed (%lu)\n", *N, NMAX);
		*N = NMAX;
	}
	
	/* Retrieving max # of memories */
	*Pmax = (unsigned long) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				 "Debe pasar el maximo # de memorias de la red "
				 "como segundo argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	/* Retrieving # of neurons */
	*hop = (unsigned long) strtol (argv[3], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "Error en la entrada, en la cadena'%s'\n"
				 "Debe pasar el salto (de incremento de "
				 "memorias) como tercer argumento\n", err);
		exit (EXIT_FAILURE);
	}
	
	return;
}





int main  (int argc, char **argv)
{
	/* We have access to the following global vars:
	unsigned long	N = 0,		# of neurons in the net
			P = 0,		# of memories in the net
			Pmax = 0,	max # of memories in the net
			hop = 0;	magnitude of P hopping till Pmax reached
	*/
	int *S = NULL, *XI=NULL;
	
	parse_input (argc, argv, &N, &Pmax, &hop);
	
	return EXIT_SUCCESS;
}
