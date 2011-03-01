#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <err.h>
#include <math.h>
#include <limits.h>
#include <float.h>
#include <gsl/gsl_vector.h>

#include "anfis.h"
#include "mzran13.h"
#include "membership_functions.h"
#include "consequent_parameters.h"



/* # de ramas de la red */
size_t  _T = 0;

/* Dimensión de entrada de la red
 * Es decir, con cuantos puntos previos se tratará de adivinar el "siguiente" */
size_t  _N = 0;

/* Salto entre puntos de entrada de la red
 * ie: 1 indica que los valores tomados serán uno, el siguiente, el siguiente
 *     M indica que se toma uno, se saltean M-1, se toma el M+1-ésimo...
 *     etc.
 */
#define  JUMP		6


/* Rango de los valores de entrada */
double  _LB = DBL_MAX,  /* Límite inferior */
	_UB = -DBL_MAX; /* Límite superior */



/* # de veces que se presentará la muestra a la red para aprendizaje */
#define  NITER   1

/* # de ejemplos de entre los datos muestrales que se usarán para aprendizaje */
#define  NTRAIN  500

/* para pretty printing de los mf finales */
#define  width   15

#define  ran()		(((double) mzran13()) / ((double) ULONG_MAX))
#define  lpow(a,b)	(lround (pow (((double) a), ((double) b))))




/* Genera un arreglo con los valores muestrales extraídos del archivo
 * especificado en los argumentos recibidos.
 *
 * La memoria alocada para 'y' le pertenece al usuario, quien deberá liberarla
 * usando la rutina standard de glibc: free()
 *
 * El valor de retorno es el arreglo generado, que tiene longitud 'nlines'
 */
static double *
get_sample_values (int argc, char **argv, size_t *nlines)
{
	int matched = 0;
	unsigned int i = 0;
	double	t = 0.0,
		f_t = 0.0,
		*y = NULL;
	char *nomfile = NULL,
	     *error = NULL;
	FILE *file = NULL;
	
	/* Obteniendo argumentos */
	nomfile = argv[3];
	*nlines = (size_t) strtol (argv[4], &error, 10);
	if (error[0] != '\0') {
		fprintf (stderr, "\aEl cuarto argumento debe ser el # de líneas"
				 " que tiene el archivo con la muestra\n");
		exit (EXIT_FAILURE);
	}
	
	/* Accediendo a los datos */
	file = fopen (nomfile,"r");
	if (file == NULL) {
		err (1, "\aArchivo de datos corrupto: '%s'\n", nomfile);
	} else {
		y = (double *) malloc (*nlines * sizeof (double));
		assert (y != NULL);
	}
	
	/* Almacenando los datos en el arreglo */
	for (i=0 ; i < *nlines ; i++) {
		
		matched = fscanf (file, "%lf %lf\n", &t, &f_t);
		
		if (feof (file) && matched != 2) {
			fprintf (stderr, "El archivo tenía una cantidad de "
					 "líneas menor a la especificada: %lu\n",
					 *nlines);
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
			if (f_t < _LB)
				_LB = f_t;
			if (f_t > _UB)
				_UB = f_t;
		}
	}
	
	fclose (file);
	
	return y;
}




/* Parsea la entrada. En caso de ser correcta:
 *
 *  · setea al # de ramas y la dimensión de entrada de la red
 *  · devuelve un array con espacio suficiente para los datos muestrales (y)
 *  · calcula la cantidad de datos muestrales (nlines)
 *  · genera el archivo de salida de las estimaciones de la red (fout)
 *  · (OP) genera el archivo de salida para los errores de aprendizaje (ferr)
 *  · (OP) genera el archivo de salida para las funciones MF iniciales (f_imf)
 *  · (OP) genera el archivo de salida para las funciones MF finales   (f_fmf)
 */
static void
parse_input (int argc, char **argv, double **y, size_t *nlines,
	      FILE **fout, FILE **ferr, FILE **f_imf, FILE **f_fmf)
{
	char *error = NULL;
	
	printf ("argc = %d\n", argc);
	if (6 > argc || argc > 9) {
		fprintf (stderr, "Debe invocar al programa con los argumentos:"
				"\n\t1) # de ramas de la red"
				"\n\t2) Dimensión de la entrada"
				"\n\t3) Nombre del archivo que contiene los "
					"datos muestrales"
				"\n\t4) # de líneas del mismo"
				"\n\t5) Nombre del archivo de salida para los "
					"valores estimados por la red"
				"\n\t6) Nombre del archivo de salida para el "
					"error de aprendizaje (OPCIONAL)"
				"\n\t7) Nombre del archivo para las funciones "
					"membresía iniciales (OPCIONAL)"
				"\n\t8) Nombre del archivo para las funciones "
					"membresía finales (OPCIONAL)\n\n");
		exit (EXIT_FAILURE);
	}
	
	_T = (size_t) strtol (argv[1], &error, 10);
	if (error[0] != '\0') {
		fprintf (stderr, "\aEl primer argumento debe ser "
				 " el # de ramas de la red\n");
		exit (EXIT_FAILURE);
	}
	
	_N = (size_t) strtol (argv[2], &error, 10);
	if (error[0] != '\0') {
		fprintf (stderr, "\aEl segundo argumento debe ser "
				 " la dimensión de entrada de la red\n");
		exit (EXIT_FAILURE);
	}
	
	*y = get_sample_values (argc, argv, nlines);
	/* Si esta rutina regresa es porque tuvo éxito rellenando 'y'
		* Además actualizó los valores de LB y UB */
	
	*fout = fopen (argv[5], "w");
	if (*fout == NULL) {
		err (1, "No se pudo crear archivo de salida\n");
	}
	
	if (argc >= 7) {
		*ferr = fopen (argv[6], "w");
		if (*ferr == NULL || ferror (*ferr)) {
			warn ("Archivo de errores de aprendizaje "
				"corrupto: '%s'\nNo se guardará registro"
				" del nivel de error\n", argv[4]);
		}
	}
	
	if (argc >= 8) {
		*f_imf = fopen (argv[7], "w");
		if (*f_imf == NULL || ferror (*f_imf)) {
			warn ("'%s': imposible abrir archivo.\nNo se graficarán"
			      " las funciones membresía iniciales\n", argv[5]);
		}
	}
	
	if (argc >= 9) {
		*f_fmf = fopen (argv[8], "w");
		if (*f_fmf == NULL || ferror (*f_fmf)) {
			warn ("'%s': imposible abrir archivo.\nNo se "
				"graficarán las funciones membresía resultantes"
				" tras el aprendizaje\n", argv[5]);
		}
	}
	
	return;
}



/* Construye un conjunto de t*n funciones membresía
 * donde el rango de valores de entrada a cubrir es 'UB'-'LB'
 *
 * Dicho rango es cubierto casi por completo en cada una de las 't' ramas
 * Sin embargo cada rama lo cubre de manera diferente a las demás.
 */
static MF_t *
gen_mfs (size_t n, size_t t, double LB, double UB)
{
	int i = 0, j = 0;
	MF_t *mf = NULL;
	double	nn = n, tt = t,
		range = (UB-LB) * RANGEXP,
		/* base = LB + (UB - LB) / 2.0 - range / 2.0 */
		base = (UB + LB - range) / 2.0,
		a = range / (2.0 * nn * tt),
		b = SLOPE,
		c = 0.0;
	
	mf = (MF_t *) malloc (t * n * sizeof (MF_t));
	assert (mf != NULL);
	
	#pragma omp parallel for private(i,c)
	for (j=0 ; j < n ; j++) {
		
		for (i=0 ; i < t ; i++) {
			double	ii = i, jj = j,
				base_i = base + ii * range/tt,
				/* centro gral. de las MF's de la i-ésima rama */
				ci = base_i + range / (2.0*tt);
			
			/* Centro de ésta MF */
			c = base + 2.0 * a * (ii * nn + jj) + a;
			/* Solapamiento entre las MF's de una misma rama */
			c += (c - ci) * (-0.2 * JUNCTION_1);
			/* Solapamiento del dominio cubierto por cada rama */
			c += (c - (base + range/2.0)) * (-0.2 * JUNCTION_2);
			
			/* Todas las MF serán campanas */
			mf[i*n+j].k = bell;
			mf[i*n+j].p[0] = a * AWIDTH;
			mf[i*n+j].p[1] = b;
			mf[i*n+j].p[2] = c + DECENTRE;
		}
	}
	
	return mf;
}



/* Si el archivo de salida para las funciones membresía finales fue especificado
 * esta función imprime en él los parámetros de todas las MFs de la red
 * que fueron modificadas con los ciclos de entrenamiento
 */
static void
mf_print (anfis_t net, FILE *fout)
{
	int i = 0, j = 0;
	MF_t mf;
	
	/* Si el archivo no es un archivo no escribimos nada */
	if (fout == NULL || ferror (fout)) {
		return;
	}
	
	for (i=0 ; i < _T ; i++) {
		for (j=0 ; j < _N ; j++) {
			assert (ANFIS_OK == anfis_get_MF (net, i, j, &mf));
			fprintf ( fout, "%d%*d\t%-*.4f%-*.4f%-*.4f\n",
				  i, width/2, j,
				  width, mf.p[0],
				  width, mf.p[1],
				  width, mf.p[2]  );
		}
	}
	/* Formato de impresión del archivo de salida:
	 * 0 0      MF[0][0].p[0]       MF[0][0].p[1]       MF[0][0].p[2]
	 * 0 1      MF[0][1].p[0]       MF[0][1].p[1]       MF[0][1].p[2]
	 * ...                 ...                 ...
	 * 0 N-1    MF[0][N-1].p[0]     MF[0][N-1].p[1]     MF[0][N-1].p[2]
	 * 1 0      MF[1][0].p[0]       MF[1][0].p[1]       MF[1][0].p[2]
	 * ...                 ...                 ...
	 * 1 N-1    MF[1][N-1].p[0]     MF[1][N-1].p[1]     MF[1][N-1].p[2]
	 * ...                 ...                 ...
	 * T-1 0    MF[T-1][0].p[0]     MF[T-1][0].p[1]     MF[T-1][0].p[2]
	 * ...                 ...                 ...
	 * T-1 N-1  MF[T-1][N-1].p[0]   MF[T-1][N-1].p[1]   MF[T-1][N-1].p[2]
	 */
	
	return;
}



/* Crea 'p' muestras (vacías) con entradas de dimensión n */
static t_sample *
sample_alloc (size_t n, size_t p)
{
	t_sample *s = NULL;
	unsigned long i = 0;
	
	s = (t_sample *) malloc (p * sizeof (t_sample));
	assert (s != NULL);
	
	for (i=0 ; i < p ; i++) {
		s[i].in = gsl_vector_alloc (n);
		assert (s[i].in != NULL);
	}
	
	return s;
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
 *     Entonces la muestra generada será: { ((0,20) , 40) }
 */
static void
sample_gen (t_sample *s, size_t n, size_t p, size_t jump, double *y)
{
	long i = 0, j = 0, k = 0;
	long *loi = NULL;	/* Lista de índices para acceder 
				 * aleatoriamente a los valores de y */
	
	assert (s != NULL);
	
	/* Primero creamos la lista de índices aleatorios */
	loi = (long *) malloc (p * sizeof (long));
	assert (loi != NULL);
	#pragma omp parallel for if(p>200)
	for (k=0 ; k < p ; k++) {
		loi[k] = k;
	}
	k = p-1;
	while (k-- > 0) {
		i = floor (ran()*p);
		loi[k] += loi[i];
		loi[i]  = loi[k] - loi[i];
		loi[k] -= loi[i];
	}
	
	/* Ahora escogemos aleatoriamente las 'p' muestras */
	#pragma omp parallel for default(shared) private(i,j)
	for (k=0 ; k < p ; k++) {
		i = loi[k];
		for (j=0 ; j < n ; j++) {
			/* Presentación secuencial de ejemplos: */
			gsl_vector_set (s[k].in, j, y[k+j*jump]);
			/* Presentación aleatoria de ejemplos: */
/*			gsl_vector_set (s[k].in, j, y[i+j*jump]);
*/		}
		s[i].out = y[i+n*jump];
	}
	
	free (loi);
	loi = NULL;
	
	return;
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
 * Los valores de salida de la red son impresos secuencialmente en el archivo
 * 'fout' Si se especificó archivo para los errores de aprendizaje ('ferr')
 * éstos también serán registrados
 */
static void
exercise_network (anfis_t net, t_sample *sample, size_t p, double offset,
		   FILE *fout, FILE *ferr)
{
	unsigned long t = 0;
	double out = 0.0;
	
	/* Primero insertamos en la red los valores de los consequent parameters
	 * que setearon en "consequent_parameters.c"
	 */
	if (cp[0] != -DBL_MAX) {
		size_t	n = anfis_get_n (net),
			t = anfis_get_t (net);
		long  i = 0;
		for (i=0 ; i < t ; i++) {
			anfis_set_P (net, i, &(cp[i*(n+1)]));
		}
	}
	
	for (t=0 ; t < p ; t++) {
		out = anfis_eval (net, sample[t].in);
		fprintf (fout, "%f\t%f\n", t + offset, out);
		if (ferr != NULL) {
			fprintf (ferr, "%.1f\t%f\n",
				 t + offset, sample[t].out - out);
		}
	}
	
	return;
}




int main (int argc, char **argv)
{
	int i = 0, error = 0, p = 0;
	size_t nlines = 0;
	FILE *fout = NULL,
	     *ferr = NULL,
	     *f_imf = NULL,
	     *f_fmf = NULL;
	double *y = NULL;
	t_sample *sample = NULL;
	MF_t *mfs = NULL;
	anfis_t net = NULL;
	
	/* Obtenemos los datos */
	parse_input (argc, argv, &y, &nlines, &fout, &ferr, &f_imf, &f_fmf);
	
	printf ("\n\nT = %zu\tN = %zu\n", _T, _N);
	/* Creamos la red */
	mfs = gen_mfs (_N, _T, _LB, _UB);
	net = anfis_create (_N, _T, mfs);
	printf ("Red ANFIS antes del aprendizaje:\n\n");
	anfis_print (net);
	
	/* Imprimimos las funciones membresía iniciales (antes del aprendizaje) */
	mf_print (net, f_imf);
	
	/** Generamos un conjunto de entrenamiento con la mitad de los datos
	 ** y entrenamos la red con él */
	sample = sample_alloc (_N, nlines);
	p = nlines / (2*NITER);
	for (i=0 ; i < NITER ; i++) {
		sample_gen (sample, _N, p, JUMP, &(y[i*p]));
		error  = anfis_train (net, sample, p);
		assert (error == ANFIS_OK);
	}
	
	printf ("\n\nRed ANFIS después del aprendizaje:\n");
	anfis_print (net);
	
	
	/** Alimentamos la red con todos los datos a disposición
	 ** para analizar qué tan bueno fue su aprendizaje */
	p = nlines - (_N+1) * JUMP;
	sample_gen (sample, _N, p, JUMP, y);
	exercise_network (net, sample, p, 118, fout, ferr);
	
	
	/* Imprimimos las funciones membresía resultantes del aprendizaje */
	mf_print (net, f_fmf);
	
	/* Limpiando memoria */
	net = anfis_destroy (net);
	free (mfs);	mfs = NULL;
	sample = sample_free (sample, p);
	free (y);	y = NULL;
	fclose (fout);	fout = NULL;
	if (ferr != NULL && fileno (ferr) >= 0) {
		fclose (ferr); ferr = NULL;
	}
	if (f_imf != NULL && fileno (f_imf) >= 0) {
		fclose (f_imf); f_imf = NULL;
	}
	if (f_fmf != NULL && fileno (f_fmf) >= 0) {
		fclose (f_fmf); f_fmf = NULL;
	}
	
	return EXIT_SUCCESS;
}
