#include <stdio.h>
#include <stdlib.h>
#include "rdg.h"

int main (int argc, char **argv)
{
	long n = 0;
	double p = 0.0;
	char *err = NULL;
	
	if (argc != 3) {
		fprintf (stderr, "Esta rutina debe invocarse con 2 argumentos:"
				"\n  1) 'n': maximo # posible de éxitos"
				"\n  2) 'p': probabilidad de un éxito\n");
		exit (EXIT_FAILURE);
	
	} else {
		
		n = strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf("Error: en el argumento #1, en: %s\n", argv[1]);
			exit (EXIT_FAILURE);
		}
		
		p = strtod (argv[2], &err);
		if (err[0] != '\0') {
			printf("Error: en el argumento #2, en: %s\n", argv[2]);
			exit (EXIT_FAILURE);
		}
		
	}
	
	print_bin (n, p);
	
	return 0;
}
