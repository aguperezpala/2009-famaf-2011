#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <gsl/gsl_vector.h>

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

/* Pendiente de las funciones membresía (parámetro 'b' de la MF tipo "bell") */
#define  slope	3

/* Rango de los valores de entrada */
double  LB = 0.0, /* Límite inferior */
	UB = 0.0; /* Límite superior */




/* Rellena el arreglo 'y' con los valores muestrales extraídos del archivo
 * especificado en los argumentos recibidos.
 *
 * La memoria alocada para 'y' le pertenece al usuario, quien deberá liberarla
 * usando la rutina standard de glibc: free()
 *
 * El valor de retorno es la longitud de 'y'
 */
static size_t
get_sample_values (char **argv, double *y)
{
	int matched = 0;
	unsigned int i = 0;
	double	t = 0.0,
		f_t = 0.0;
	char *nomfile = NULL,
	     *err = NULL;
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
	
	/* Accediendo a los datos */
	file = fopen (nomfile,"r");
	if (file == NULL) {
		fprintf ("Imposible acceder al archivo especificado: %s\n",
			 nomfile);
		exit (EXIT_FAILURE);
	} else {
		y = (double *) malloc (nlines * sizeof (double));
		assert (y != NULL);
	}
	
	/* Almacenando los datos en el arreglo */
	for (i=0 ; i < nlines ; i++) {
		
		matched = fscanf (file, "%f %f\n", &t, &f_t);
		
		if (matched == EOF) {
			fprintf (stderr, "El archivo tenía una cantidad de "
					 "líneas menor a la especificada: %z\n",
					 nlines);
			exit (EXIT_FAILURE);
		
		} else if (matched != 2) {
			fprintf (stderr, "El archivo debe contener sólo dos "
				 "columnas: la izquierda con los tiempos y la "
				 "derecha con los valores de la serie/función "
				 "para esos tiempos\n");
			exit (EXIT_FAILURE);
		
		} else {
			/* Todo en orden, almacenamos el valor */
			y[i] = f_t;
			if (f_t < LB)
				LB = floor (f_t);
			if (f_t > UB)
				UB = ceil (f_t);
		}
	}
	
	fclose (file);
	
	return nlines;
}




/* Construye un conjunto de t*n funciones membresía
 * donde el rango de valores de entrada a cubrir es 'UB'-'LB'
 */
static MF_t *
gen_mfs (size_t n, size_t t, size_t LB, size_t UB)
{
	unsigned int i = 0, j = 0;
	MF_t *mf = NULL;
	double	a = ((double)(UB-LB)) / ((double)2.0*n),
		b = slope,
		c = 0.0;
	
	mf = (MF_t *) malloc (t * sizeof (MF_t));
	assert (mf != NULL);
	
	#pragma omp parallel for default(shared) private(j,i,c)
	for (j=0 ; j < n ; j++) {
		c = ((double) UB / (double) n) * ((double) j) + a;
		for (i=0 ; i < t ; i++) {
			/* Todas las MF serán campanas */
			MF[i*n+j].k = bell;
			MF[i*n+j].p[0] = a;
			MF[i*n+j].p[1] = b;
			MF[i*n+j].p[2] = c;
		}
	}
	
	return mf;
}




/* Genera un conjunto de entrenamiento a partir de los datos de y,
 * que contiene 'p' datos de tipo t_sample (ie: pares (input,output))
 *
 * Los inputs de cada t_sample son vectores de 'n' datos tomados de 'y'
 * Si el primer elemento de uno de estos inputs es y[i], entonces el siguiente
 * será y[i+jump] y así sucesivamente.
 * Es decir que los datos son tomados de 'y' mediante saltos de longitud 'jump'
 *
 * El output es el dato "siguiente" al del último valor de input,
 * donde "siguiente" también implica un salto de longitud 'jump'
 *
 * Ej: p = 1, n = 2 , jump = 4 , y = [0,5,10,15,20,25,30,35,40,45]
 *
 *     Entonces el resultado devuelto será: { ((0,20) , 40) }
 */
static t_sample *
gen_sample (double *y, size_t p, size_t n, size_t jump)
{
	unsigned int i = 0, j = 0;
	t_sample *s = NULL;
	
	s = (t_sample *) malloc (p * sizeof (t_sample));
	assert (s != NULL);
	
	#pragma omp parallel for default(shared) private(i,j)
	for (i=0 ; i < p ; i++) {
		s[i].in = gsl_vector_alloc (n);
		assert (s[i].in != NULL);
		
		for (j=0 ; j < n ; j++) {
			gsl_vector_set (s[i].in, j, y[i+j*jump]);
		}
		s[i].out = y[i+n*jump];
	}
	
	return sample;
}



/* Libera la memoria de las 'p' muestras contenidas en 's' */
static t_sample *
sample_free (t_sample *s, size_t p)
{
	unsigned long i = 0;
	
	assert (s != NULL);
	
	for (i=0 ; i < p ; i++) {
		gsl_vector_free (s[i].in);
		s[i].in = NULL;
	}
	
	free (s);
	s = NULL;
	
	return s;
}



/* Ejercita la red 'p' veces con los valores de 's'
 *
 * Para ello alimenta cada entrada s[i].in a la red y compara el valor devuelto
 * con el resultado verdadero (s[i].out)
 *
 * Los errores de aprendizaje son impresos secuencialmente en el archivo 'fout'
 */
static void
exercise_network (anfis_t net, t_sample *sample, size_t p, FILE *fout);
{
	unsigned long t = 0;
	double out = 0.0;
	
	for (t=0 ; t < p ; t++) {
		out = anfis_eval (net, s[0].in);
		fprintf (fout, "%f\n", out - s[0].out);
	}
	
	return;
}




int main (int argc, char **argv)
{
	int i = 0, error = 0;
	size_t nlines = 0;
	FILE *fout = NULL;
	double *y = NULL;
	t_sample *sample = NULL;
	MF_t *mfs = NULL;
	
	
	if (argc != 4) {
		fprintf (stderr, "Debe invocar al programa con los argumentos:"
				 "\n\t1) Nombre del archivo que contiene los "
				 "datos muestrales\n\t2) # de líneas del mismo"
				 "\n\t3) Nombre del archivo de salida del error"
				 " de aprendizaje obtenido por la red\n\n");
		exit (EXIT_FAILURE);
		
	} else {
		nlines = get_sample_values (argv, y);
		/* Si esta rutina regresa es porque tuvo éxito rellenando 'y'
		 * Además actualizó los valores de LB y UB */
		
		fout = fopen (argv[3], "w");
		if (fout == NULL) {
			fprintf ("No se pudo crear archivo de salida\n");
			exit (EXIT_FAILURE);
		}
	}
	
	
	
	/* Generamos un conjunto de entrenamiento con la mitad de los datos 
	* y entrenamos la red con él */
	sample = gen_sample (y, nlines/2, N, JUMP);
	
	mfs = gen_mfs (N, T, LB, UB);
	net = anfis_create (N, T, mfs);
	error  = anfis_train (net, sample, nlines/2);
	assert (error == ANFIS_OK);
	
	
	/* Alimentamos la otra mitad de los datos a la red para analizar
	 * qué tan bueno fue su aprendizaje */
	sample = sample_free (sample, nlines/2);
	sample = gen_sample (&(y[nlines/2]), nlines/2, N, JUMP);
	exercise_network (net, sample, nlines/2, fout);
	
	
	/* Limpiando memoria */
	net = anfis_destroy (net);
	free (mfs);	mfs = NULL;
	sample = sample_free (sample, nlines/2);
	free (y);	y = NULL;
	fclose (fout);
	
	return EXIT_SUCCESS;
}
