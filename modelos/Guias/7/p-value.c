#include <stdio.h>
#include <stdlib.h>
#include "rdg.h"

int main (int argc, char **argv)
{
	double	t = 0.0;	/* Valor del estadistico de la muestra */
	int	v = 0;		/* Grados de libertad de la Ji-2 */
	double	p = 0.0;	/* p-valor asociado a t (segun Ji-2) */
	char *err = NULL;
	
	if (argc != 3) {
		fprintf (stderr, "Esta rutina debe invocarse con 2 argumentos:"
				"\n  1) valor del estadistico de la muestra"
				"\n  2) grados de libertad de la Ji-2\n");
		exit (EXIT_FAILURE);
	
	} else {
		
		t = strtod (argv[1], &err);
		if (err[0] != '\0') {
			printf("Error: en el argumento #1, en: %s\n", argv[1]);
			exit (EXIT_FAILURE);
		}
		
		v = strtol (argv[2], &err, 10);
		if (err[0] != '\0') {
			printf("Error: en el argumento #2, en: %s\n", argv[2]);
			exit (EXIT_FAILURE);
		}
	}
	
	p = chi_cuadrada (v, t);
	
	printf ("p-valor = %.8f\n", p);
	
	return 0;
}
