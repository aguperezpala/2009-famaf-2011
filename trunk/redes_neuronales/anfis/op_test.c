#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <math.h>

#include "mzran13.h"
#include "anfis.h"


/* Tipo de las funciones de membresía */
#define  MF_KIND	bell


/* Dimensión máxima de entrada de la red */
#define  N_MAX		64
/* Tamaño del salto de dimensión de entrada */
#define  N_HOP		16


/* Máxima cantidad de ramas paralelas en la red */
#define  T_MAX		16
/* Tamaño del salto en el # de ramas */
#define  T_HOP		4


/* # de ejemplos que tendrá un training set */
#define  P		((3*N_MAX*T_MAX)/2)


/* # de épocas (de entrenamiento) antes de empezar a medir */
#define  NEPOCHS	100


/* Rango de valores para las entradas */
#define  LBOUND		(-2.0)
#define  UBOUND		(2.0)


#define  ran()		(((double) mzran13()) / ((double) ULONG_MAX))




static MF_t *
init_MF (size_t n, size_t t)
{
	size_t i = 0;
	MF_t *mf = NULL;
	
	mf = (MF_t *) malloc ( t*n * sizeof (MF_t));
	assert (mf != NULL);
	
	/* Construimos las funciones de membresía ... */
	for (i=0 ; i < t*n ; i++) {
		
		/* ... con una función predeterminada ... */
		mf[i].k = MF_KIND;
		
		/* ... y parámetros cualesquiera */
		mf[i].p[0] = ran() * (UBOUND - LBOUND) * 0.25;
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
	unsigned long i = 0;
	
	s = (t_sample *) malloc (p * sizeof (t_sample));
	assert (s != NULL);
	
	for (i=0 ; i < p ; i++) {
		s[i].in = gsl_vector_alloc (n);
		assert (s[i].in != NULL);
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
	unsigned int i = 0;
	double rand = 0.0;
	
	assert (s != NULL);
	
	for (i=0 ; i < p ; i++) {
		rand = ran() * (UBOUND - LBOUND) + LBOUND;
		gsl_vector_set_all (s[i].in, rand);
		s[i].out = rand*rand;
	}
	
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
	printf ("Error promedio de generalización: %f\n", err / (double) times);
	
	s = sample_free (s, 1);
	
	return;
}




int main (void)
{
	unsigned long n = 0, t = 0, e = 0;
	int res = ANFIS_ERR;
	MF_t *mf = NULL;
	t_sample *s = NULL;
	anfis_t net = NULL;
	
	printf ("Inicio del test operacional\n\n");
	
	printf ("Aspecto de una red:\n");
	mf  = init_MF (N_HOP, T_HOP);
	net = anfis_create (N_HOP, T_HOP, mf);
	anfis_print (net);
	free (mf);	mf = NULL;
	net = anfis_destroy (net);
	printf ("\n");
	
	/* Para cada # de ramas especificado */
	for (t = T_HOP ; t < T_MAX ; t += T_HOP) {
		
		/* Para cada dimensión de entrada especificada */
		for (n = N_HOP ; n < N_MAX ; n += N_HOP) {
			
			printf ("\nEjercitando red con:"
				"\t%lu ramas\t%lu entradas\n", t, n);
			
			/* Creamos espacio de trabajo */
			mf  = init_MF (n, t);
			s   = sample_alloc (n, P);
			net = anfis_create (n, t, mf);
			
			/* Entrenamos la red "NEPOCHS" veces */
			for (e=0 ; e < NEPOCHS ; e++) {
				sample_gen (s, n, P);
				res = anfis_train (net, s, P);
				assert (res == ANFIS_OK);
			}
			
			/* Ejercitamos la red otras "NEPOCHS" veces */
			exercise_network (net, n, NEPOCHS);
			
			net = anfis_destroy (net);
			s   = sample_free (s, P);
			free (mf);	mf = NULL;
			
			printf ("\n");
		}
	}
	
	return EXIT_SUCCESS;
}