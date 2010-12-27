#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "anfis.h"


/* # de ramas de la red */
#define  T	2

/* Dimensión de entrada de la red
 * Es decir, con cuantos puntos previos se tratará de adivinar el "siguiente" */
#define  N	4

/* Salto entre puntos de entrada de la red
 * ie: 1 indica que los valores tomados serán uno, el siguiente, el siguiente
 *     2 indica que se toma uno, se saltea otro, se toma el siguiente...
 *     etc.
 */
#define  JUMP	6




/* Rellena el arreglo 'y' con los valores muestrales extraídos del archivo
 * especificado en los argumentos recibidos.
 *
 * La memoria alocada para 'y' le pertenece al usuario, quien deberá liberarla
 * usando la rutina standard de glibc: free()
 */
static double *
get_sample_values (char **argv)
{
	int matched = 0;
	unsigned int i = 0;
	double	t = 0.0,
		f_t = 0.0,
		*y = NULL;
	char *nomfile = NULL,
	     *err = NULL,
	     *line = NULL;
	size_t nlines = 0;
	FILE *file = NULL;
	
	/* Obteniendo argumentos */
	nomfile = argv[1];
	nlines = (size_t) strtol (argv[2], &err, 10);
	if (err[0] != '\0') {
		fprintf (stderr, "El segundo argumento debe ser el # de líneas "
				 "que tiene el archivo con la muestra\n", err);
		exit (EXIT_FAILURE);
	}
	
	file = fopen (nomfile,"r");
	if (file == NULL) {
		fprintf ("Imposible acceder al archivo especificado: %s\n",
			 nomfile);
		exit (EXIT_FAILURE);
	} else {
		y = (double *) malloc (nlines * sizeof (double));
		assert (y != NULL);
	}
	
	for (i=0 ; i < nlines ; i++) {
		matched = fscanf (file, "%f %f\n", &t, &f_t);
		if (matched != 2) {
			fprintf (stderr, "El archivo debe contener sólo dos "
				 "columnas: la izquierda con los tiempos y la "
				 "derecha con los valores de la serie/función "
				 "para esos tiempos\n");
				 exit (EXIT_FAILURE);
		}
	}
	
	fclose (file);
	
	return y;
}


int main (int argc, char **argv)
{
	double *y = NULL;
	t_sample *sample = NULL;
	
	
	if (argc != 3) {
		fprintf (stderr, "Debe invocar al programa con los argumentos:"
				 "\n\t1) Nombre del archivo que contiene los "
				 "datos muestrales\n\t2) # de líneas del mismo\n\n");
		exit (EXIT_FAILURE);
	} else {
		y = get_sample_values (argv);
	}
	
	sample = gen_sample (y);
	
	return EXIT_SUCCESS;
}
