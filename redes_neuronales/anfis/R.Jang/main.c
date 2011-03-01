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



/* Dimensión de entrada de la red
 * O sea, con cuantos puntos "previos" se tratará de adivinar "el siguiente" */
size_t  _N = 0;

/* # de interpretaciones para cada uno de los _N elementos de entrada */
size_t  _T = 0;

/* Salto entre puntos de entrada de la red
 * ie: 1 indica que los valores tomados serán uno, el siguiente, el siguiente
 *     M indica que se toma uno, se saltean M-1, se toma el M+1-ésimo..., etc
 */
#define  JUMP	6


/* Rango de los valores de entrada */
double  _LB = DBL_MAX,  /* Límite inferior */
	_UB = -DBL_MAX; /* Límite superior */



/* # de veces que se presentará la muestra a la red para aprendizaje */
#define  NITER   1

/* para pretty printing de los mf finales */
#define  width   15

#define  ran()		(((double) mzran13()) / ((double) ULONG_MAX))
#define  _byte_size	(1<<3)
#define  MSB		(_byte_size*sizeof(size_t)-1)
#define  int_val(x)	((x) & (~(((size_t) 1) << MSB)))
#define  lpow(a,b)	(lround (pow (((double) a), ((double) b))))




/* Genera un arreglo con los valores muestrales extraídos del archivo
 * especificado en los argumentos recibidos.
 *
 * La memoria alocada para 'y' le pertenece al usuario, quien deberá liberarla
 * usando la rutina standard de glibc: free()
 *
 * El valor de retorno es el arreglo generado, que tiene longitud 'nlines'
 *
 * USO: y = get_sample_values (argc, argv, &nlines)
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
		fprintf (stderr, "\aEl segundo argumento debe ser el # de líneas"
				 " que tiene el archivo con la muestra\n");
		exit (EXIT_FAILURE);
	}
	
	/* Accediendo a los datos */
	file = fopen (nomfile,"r");
	if (file == NULL || ferror(file)) {
		err (1, "\aNo se pudo abrir el archivo de datos '%s'\n", nomfile);
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
				 "columnas: la izquierda con los tiempos\ny "
				 "la derecha con los valores de la serie/"
				 "función para esos tiempos\n");
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
 *  · setea la dimensión de entrada y el # de interpretaciones (ie: _N y _T)
 *  · guarda en 'y' los datos muestrales
 *  · calcula la cantidad de datos muestrales (nlines)
 *  · genera el archivo de salida para las estimaciones de la red (fout)
 *  · (OP) genera el archivo de salida para los errores de aprendizaje (ferr)
 *  · (OP) genera el archivo de salida para las funciones MF iniciales (f_imf)
 *  · (OP) genera el archivo de salida para las funciones MF finales   (f_fmf)
 */
static void
parse_input (int argc, char **argv, double **y, size_t *nlines,
	      FILE **fout, FILE **ferr, FILE **f_imf, FILE **f_fmf)
{
	char *error = NULL;
	
	if (6 > argc || argc > 9) {
		fprintf (stderr, "Debe invocar al programa con los argumentos:"
				"\n\t1) Dimensión de la entrada"
				"\n\t2) # de interpretaciones para cada "
					"elemento de la entrada"
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
					"membresía finales   (OPCIONAL)"
				"\n\t9) Nombre del archivo con los conseq.param"
					". para usar tras el aprendizaje\n\n");
		exit (EXIT_FAILURE);
	}
	
	_N = (size_t) strtol (argv[1], &error, 10);
	if (error[0] != '\0') {
		fprintf (stderr, "\aEl primer argumento debe ser "
				 "la dimensión de entrada de la red\n");
		exit (EXIT_FAILURE);
	}
	
	_T = (size_t) strtol (argv[2], &error, 10);
	if (error[0] != '\0') {
		fprintf (stderr, "\aEl segundo argumento debe ser el # de "
				 "interpretaciones para cada elemento de entrada\n");
		exit (EXIT_FAILURE);
	}
	
	*y = get_sample_values (argc, argv, nlines);
	/* Si esta rutina regresa es porque tuvo éxito rellenando 'y'
	 * Además actualizó los valores de LB y UB */
	
	*fout = fopen (argv[5], "w");
	if (*fout == NULL || ferror (*fout)) {
		err (1, "No se pudo crear archivo de salida\n");
	}
	
	if (argc >= 7) {
		*ferr = fopen (argv[6], "w");
		if (*ferr == NULL || ferror (*ferr)) {
			warn ("'%s': imposible abrir archivo.\nNo se guardará "
			      "registro de los errores de aprendizaje\n", argv[6]);
		}
	}
	
	if (argc >= 8) {
		*f_imf = fopen (argv[7], "w");
		if (*f_imf == NULL || ferror (*f_imf)) {
			warn ("'%s': imposible abrir archivo.\nNo se graficarán"
			      " las funciones membresía iniciales\n", argv[7]);
		}
	}
	
	if (argc >= 9) {
		*f_fmf = fopen (argv[8], "w");
		if (*f_fmf == NULL || ferror (*f_fmf)) {
			warn ("'%s': imposible abrir archivo.\nNo se "
				"graficarán las funciones membresía resultantes"
				" tras el aprendizaje\n", argv[8]);
		}
	}

	return;
}




/* Construye un conjunto de n*t funciones membresía.
 * Es decir que por cada uno de los 'n' elementos de entrada existen 't' MFs.
 *
 * El rango de valores de entrada a cubrir es 'UB'-'LB'
 * Dado un elemento específico de la entrada dicho rango se divide más o menos
 * uniformemente entre sus 't' interpretaciones, para así cubrirlo por completo.
 */
static MF_t *
gen_mfs (size_t n, size_t t, double LB, double UB)
{
	int i = 0, j = 0;
	MF_t *mf = NULL;
	double	range = (UB-LB) * RANGEXP,
		/* base = LB + (UB - LB) / 2.0 - range / 2.0 */
		base = (UB + LB - range) / 2.0,
		a = range / (2.0 * ((double) t)),
		b = SLOPE,
		c = 0.0;
	
	assert (a != 0.0);
	
	mf = (MF_t *) malloc (n * t * sizeof (MF_t));
	assert (mf != NULL);
	
	for (j=0 ; j < t ; j++) {
		c = base + (1.0 + 2.0 * j) * a;
		c += (((double) j) - ((double) t) / 2.0) * (-0.1 * JUNCTION);
		for (i=0 ; i < n ; i++) {
			mf[i*t+j].k = bell;
			mf[i*t+j].p[0] = a * AWIDTH;
			mf[i*t+j].p[1] = b;
			mf[i*t+j].p[2] = c + DECENTRE;
		}
	}
	
	return mf;
}




/* Imprime en el archivo de salida especificado
 * los parámetros de todas las funciones membresía de la red
 */
static void
mf_print (anfis_t net, FILE *fout, size_t n, size_t t)
{
	int i = 0, j = 0, res = ANFIS_ERR;
	MF_t mf;
	
	/* Si el archivo está cagado no escribimos nada */
	if (fout == NULL || ferror (fout)) {
		return;
	}
	
	for (i=0 ; i < n ; i++) {
		for (j=0 ; j < t ; j++) {
			
			res = anfis_get_MF (net, i, j, &mf);
			assert (res == ANFIS_OK);
			
			fprintf ( fout, "%d%*d\t%-*.4f%-*.4f%.4f\n",
				  i, width/2, j,
				  width, mf.p[0],
				  width, mf.p[1],
				  mf.p[2]  );
		}
	}
	/* Formato de impresión del archivo de salida:
	 * 0 0      MF[0][0].p[0]       MF[0][0].p[1]       MF[0][0].p[2]
	 * 0 1      MF[0][1].p[0]       MF[0][1].p[1]       MF[0][1].p[2]
	 * ...                 ...                 ...
	 * 0 T-1    MF[0][T-1].p[0]     MF[0][T-1].p[1]     MF[0][T-1].p[2]
	 * 1 0      MF[1][0].p[0]       MF[1][0].p[1]       MF[1][0].p[2]
	 * ...                 ...                 ...
	 * 1 T-1    MF[1][T-1].p[0]     MF[1][T-1].p[1]     MF[1][T-1].p[2]
	 * ...                 ...                 ...
	 * N-1 0    MF[N-1][0].p[0]     MF[N-1][0].p[1]     MF[N-1][0].p[2]
	 * ...                 ...                 ...
	 * N-1 T-1  MF[N-1][T-1].p[0]   MF[N-1][T-1].p[1]   MF[N-1][T-1].p[2]
	 */
	
	return;
}



/* Crea 'p' muestras (vacías) con entradas de dimensión n */
static t_sample *
sample_alloc (size_t n, size_t p)
{
	t_sample *s = NULL;
	long k = 0;
	
	s = (t_sample *) malloc (p * sizeof (t_sample));
	assert (s != NULL);
	
	#pragma omp parallel for
	for (k=0 ; k < int_val(p) ; k++) {
		s[k].in = gsl_vector_alloc (n);
		assert (s[k].in != NULL);
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
	
	/* Ahora escogemos aleatoriamente (o no) las 'p' muestras */
	#pragma omp parallel for default(shared) private(i,j)
	for (k=0 ; k < p ; k++) {
		i = loi[k];
		for (j=0 ; j < n ; j++) {
			/* Presentación secuencial de ejemplos: */
			gsl_vector_set (s[k].in, j, y[k+j*jump]);
			/* Presentación aleatoria de ejemplos: */
/*			gsl_vector_set (s[k].in, j, y[i+j*jump]);
*/		}
		s[k].out = y[k+j*jump];
	}
	
	free (loi);
	loi = NULL;
	
	return;
}




/* Libera la memoria de las 'p' muestras contenidas en 's' */
static t_sample *
sample_free (t_sample *s, size_t p)
{
	unsigned long k = 0;
	
	assert (s != NULL);
	
	for (k=0 ; k < p ; k++) {
		gsl_vector_free (s[k].in);
		s[k].in = NULL;
	}
	
	free (s);
	s = NULL;
	
	return s;
}




/* Ejercita la red 'p' veces con los valores de 's'
 *
 * Para ello alimenta cada entrada s[k].in a la red y compara el valor devuelto
 * con el resultado verdadero (s[k].out)
 *
 * Las salidas de la red son impresas secuencialmente en el archivo 'fout'
 * Si se especificó un nombre de archivo para los errores de aprendizaje
 * ('ferr') éstos también serán registrados
 */
static void
exercise_network (const anfis_t net, const t_sample *sample,
		   size_t p, double offset, FILE *fout, FILE *ferr)
{
	unsigned long k = 0;
	double out = 0.0;
	
	/* Primero insertamos en la red los valores de los consequent parameters
	 * que setearon en "consequent_parameters.c"
	 */
	if (cp != NULL) {
		size_t	n = anfis_get_n (net),
			t = anfis_get_t (net);
		long  i = 0, M = lpow (t, n);
		for (i=0 ; i < M ; i++) {
			anfis_set_P (net, i, &(cp[i*(n+1)]));
		}
	}
	
	for (k=0 ; k < p ; k++) {
		out = anfis_eval (net, sample[k].in);
		fprintf (fout, "%f\t%f\n", k + offset, out);
		if (ferr != NULL && !ferror (ferr)) {
			fprintf (ferr, "%.1f\t%f\n",
				 k + offset, sample[k].out - out);
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
	double	*y  = NULL;
	t_sample *sample = NULL;
	MF_t *mfs = NULL;
	anfis_t net = NULL;
	
	/* Obtenemos los datos */
	parse_input (argc, argv, &y, &nlines, &fout, &ferr, &f_imf, &f_fmf);
	
	/* Creamos la red */
	mfs = gen_mfs (_N, _T, _LB, _UB);
	net = anfis_create (_N, _T, mfs);
	printf ("Red ANFIS antes del aprendizaje:\n");
	anfis_print (net);
	
	/* Imprimimos las funciones membresía iniciales (antes del aprendizaje) */
	mf_print (net, f_imf, _N, _T);
	
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
	mf_print (net, f_fmf, _N, _T);
	
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
