#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#include "mzran13.h"
#include "anfis.h"


/* Tipo de las funciones de membresía */
#define  MF_KIND	bell


/* Dimensión máxima de entrada de la red */
#define  N_MAX		4
/* Tamaño del salto de dimensión de entrada */
#define  N_HOP		4


/* Máxima cantidad de MFs (aka: interpretaciones) por elemento de entrada */
#define  T_MAX		2
/* Tamaño del salto en el # de interpretaciones */
#define  T_HOP		1


/* # de ejemplos que tendrá un training set */
#define  P		(pow (T_MAX, N_MAX) * (N_MAX + 1) + 1)


/* # de épocas (de entrenamiento) antes de empezar a medir */
#define  NEPOCHS	10


/* Rango de valores para las entradas */
#define  LBOUND		(0.0)
#define  UBOUND		(2.0)


#define  ran()		(((double) mzran13()) / ((double) ULONG_MAX))




static MF_t *
init_MF (size_t n, size_t t)
{
	size_t i = 0;
	MF_t *mf = NULL;
	
	mf = (MF_t *) malloc ( n * t * sizeof (MF_t));
	assert (mf != NULL);
	
	/* Construimos las funciones de membresía ... */
	for (i=0 ; i < n*t ; i++) {
		
		/* ... con una función predeterminada ... */
		mf[i].k = MF_KIND;
		
		/* ... y parámetros cualesquiera */
		mf[i].p[0] = ran() * (UBOUND - LBOUND) +
			     0.25  * (UBOUND - LBOUND);
		mf[i].p[1] = ran();
		mf[i].p[2] = ran() * (UBOUND - LBOUND) + LBOUND;
		
		/** NOTE esos valores son útiles sólo para funciones "bell" */
	}
	
	return mf;
}



/* Crea 'p' muestras con entradas de dimensión n */
static t_sample *
sample_alloc (size_t n, size_t p)
{
	t_sample *s = NULL;
	unsigned long k = 0;
	
	s = (t_sample *) malloc (p * sizeof (t_sample));
	assert (s != NULL);
	
	for (k=0 ; k < p ; k++) {
		s[k].in = gsl_vector_alloc (n);
		assert (s[k].in != NULL);
	}
	
	return s;
}



/* Para cada una de las 'p' muestras (con entradas de dimensión 'n')
 * se rellenan todos los lugares del input con un mismo # aleatorio,
 * y la salida con el cuadrado de dicho #
 */
static void
sample_gen (t_sample *s, size_t n, size_t p)
{
	unsigned long k = 0;
	double rand = 0.0;
	
	assert (s != NULL);
	
	for (k=0 ; k < p ; k++) {
		rand = ran() * (UBOUND - LBOUND) + LBOUND;
		gsl_vector_set_all (s[k].in, rand);
		s[k].out = rand*rand;
	}
	
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



static void
exercise_network (anfis_t net, size_t n, size_t times)
{
	t_sample *s = NULL;
	unsigned long t = 0;
	double out = 0.0, err = 0.0;
	
	s = sample_alloc (n, 1);
	
	err = 0.0;
	for (t=0 ; t < times ; t++) {
		sample_gen (s, n, 1);
		out = anfis_eval (net, s[0].in);
		err += fabs (out - s[0].out);
	}
	printf ("\nError promedio de generalización: %f\n", err / (double) times);
	
	s = sample_free (s, 1);
	
	return;
}




int main (void)
{
	unsigned long n = 0, t = 0, e = 0, p = 0;
	int res = ANFIS_ERR;
	double start = 0.0, end = 0.0;
	MF_t *mf = NULL;
	t_sample *s = NULL;
	anfis_t net = NULL;
	
	printf ("\nInicio del test operacional\n\n");
	
	printf ("Aspecto de una red:\n");
	mf  = init_MF (N_HOP, T_HOP);
	net = anfis_create (N_HOP, T_HOP, mf);
	anfis_print (net);
	net = anfis_destroy (net);
	free (mf);	mf = NULL;
	printf ("\n");
	
	start = omp_get_wtime ();
	/* Para cada dimensión de entrada especificada */
	for (n = N_HOP ; n <= N_MAX ; n += N_HOP) {
		
		/* Para cada # de interpretaciones por elemento de entrada */
		for (t = T_HOP ; t <= T_MAX ; t += T_HOP) {
			
			printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
				"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"
				"Ejercitando red con:  [%lu entrada%s]  "
				"[%lu interpretaci%s]\n",
				n, (n>1)?"s":"", t, (t>1)?"ones":"ón");
			
			/* Creamos espacio de trabajo */
			p   = lround (pow (t, n)) * (n + 1) + 1;
			mf  = init_MF (n, t);
			s   = sample_alloc (n, p);
			net = anfis_create (n, t, mf);
			res = ANFIS_OK;
			
			/* Entrenamos la red "NEPOCHS" veces */
			for (e=0 ; e < NEPOCHS && res == ANFIS_OK ; e++) {
				sample_gen (s, n, p);
				printf ("\rAlimentando ejemplo # %lu", e);
				fflush (stdout);
				res = anfis_train (net, s, p);
			}
			
			if (res != ANFIS_OK) {
				fprintf (stderr, "Error al entrenar la red, "
					 "se salteará este caso\n");
				continue;
			}
			
			/* Ejercitamos la red otras "NEPOCHS" veces */
			exercise_network (net, n, NEPOCHS);
			
			net = anfis_destroy (net);
			s   = sample_free (s, p);
			free (mf);	mf = NULL;
			
			printf ("\n");
		}
	}
	end = omp_get_wtime();
	
	printf ("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
		"~~~~~~~~~~~~~~~~~~~~~~~~\n\nTiempo transcurrido: %f s\n"
		"Fin de los tests\n\n", end-start);
	
	
	return EXIT_SUCCESS;
}
