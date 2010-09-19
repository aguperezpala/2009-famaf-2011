/*
 * Simulacion de una red de Hopfield deterministica
 *
 * Se comparan dos formas de almacenamiento de los datos de la red:
 * a) Un elemento por posicion del arreglo:
 *	En las neuronas S[i] y las memorias XI[i] esto significa que cada
 *	posicion S[i],XI[i] ∈ {+1,-1}
 *	Se refiere a estos arreglos con la notacion _pos
 * b) Un elemento por cada bit de cada posicion del arreglo:
 *	En las neuronas S[i] y las memorias XI[i] esto significa que cada
 *	bit de cada posicion S[i],XI[i] ∈ {+1,-1}
 *	Se refiere a estos arreglos con la notacion _bit
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <inttypes.h>
#include <omp.h>
#include "../mzran13.h"

#define  _byte_size  (1<<3)
#define  MOD  (_byte_size*sizeof(unsigned long))

/* N y P deben ser multiplos de MOD */
#define  N  (1<<13)
#define  P  (N/2)
#define  NU  0
#define  MAX_ITER  (1<<10)


/* Normalización a (-1,+1) */
#define  norm(x)  ((x) > 0 ? 1 : -1)

/* Acceso a una matriz de dimensiones [P][N] */
#define  AT(i,j)  ((i)*P + (j)%N)



/* Print n as a binary number */
static void
printbits (uint64_t n)
{
	uint64_t l = ((uint64_t)1) << 63;
	short k = 1;
	
	while (l > 0) {
		printf("%c", n&l ? '1' : '0');
		l >>= 1;
		!(k++ % _byte_size) ? printf(" ") : 0;
	}
	printf ("\n");
}




/* Inicializa aleatoriamente las p memorias en XIbit
 * Considera que cada componente ξ[supra-μ][sub-i] ocupa 1 bit de espacio
 * PRE: XIbit != NULL
 *	XIbit es una matriz [p][n] (ie: con 'p' memorias de 'n' componentes c/u)
 */
static void
init_XIbit (unsigned long *XIbit, unsigned int p, unsigned int n)
{
	int i = 0;
	
	assert (XIbit != NULL);
	
	/* NOTE El no determinismo del scheduler se escurre por aqui en la
	 * paralelizacion del ciclo, haciendo que las memorias NO se generen
	 * de manera identica entre llamadas sucesivas al programa.
	 *	Es decir: invocaciones consecutivas del programa resultaran
	 * en resultados de XI[] diferentes cada vez.
	 */
	#pragma omp parallel for shared(XIbit)
	for (i=0 ; i<p*n ; i++) {
		XIbit[i] = (unsigned long) mzran13();
	}
	
	return;
}


/* Inicializa el estado 'Sbit' en una posicion aleatoriamente cercana a XI[nu]
 * Considera que cada componente Sbit[sub-i] ocupa 1 bit de espacio
 * PRE: Sbit != NULL
 *	Sbit es una vector de dimension [n]
 *	Sbit[i] == 0 para todo 0 <= i < n
 *	XIbit != NULL
 *	XIbit es una matriz [p][n] (ie: con 'p' memorias de 'n' componentes c/u)
 */
static void
init_Sbit (unsigned long *Sbit , unsigned int n,
	   unsigned long *XIbit, unsigned int p, unsigned int nu)
{
	int i = 0, j = 0, base = (int) nu*n;
	unsigned long tmp = 0;
	
	assert  (Sbit != NULL);
	assert (XIbit != NULL);
	assert (nu < p);
	
	#pragma omp parallel for default(shared) private(i,j,tmp)
	for (i=0 ; i<n ; i++) {
		for (j=0 ; j<MOD ; j++) {
			if (mzran13() % 2)
				/* tmp = ξ[nu][j] */
				tmp = ((unsigned long)1) << (MOD-1-j);
			else
				/* tmp = ξ[nu][1] */
				tmp = ((unsigned long)1) << (MOD-1);
			tmp &= XIbit[base+i];
			Sbit[i] |= tmp;
		}
	}
	
	return;
}



/* Inicializa aleatoriamente las p memorias en XIpos
 * Considera que cada componente ξ[supra-μ][sub-i] ocupa 1 espacio del arreglo
 * PRE: XIpos != NULL
 *	XIbit es una matriz [p][n] (ie: con 'p' memorias de 'n' componentes c/u)
 */
static void
init_XIpos (long *XIpos, unsigned int p, unsigned int n)
{
	long i = 0;
	int aux = 0;
	
	assert (XIpos != NULL);
	
	/* NOTE El no determinismo del scheduler se escurre por aqui en la
	 * paralelizacion del ciclo, haciendo que las memorias NO se generen
	 * de manera identica entre llamadas sucesivas al programa.
	 *	Es decir: invocaciones consecutivas del programa resultaran
	 * en resultados de XI[] diferentes cada vez.
	 */
	#pragma omp parallel for shared(XIpos) private(aux)
	for (i=0 ; (unsigned long) i < n*p ; i++) {
		/* Doble modulo porque sino obtenemos ...01010101... */
		aux = (mzran13() % 5) % 2;
		XIpos[i] = norm(aux);
	}
	
	return;
}




/* Inicializa el estado 'Spos' en una posicion aleatoriamente cercana a XI[nu]
 * Considera que cada componente Spos[sub-i] ocupa 1 espacio del arreglo
 * PRE: Spos != NULL
 *	Spos es una vector de dimension [n]
 *	Spos[i] == 0 para todo 0 <= i < n
 *	XIpos != NULL
 *	XIpos es una matriz [p][n] (ie: con 'p' memorias de 'n' componentes c/u)
 */
static void
init_Spos (long *Spos , unsigned int n,
	   long *XIpos, unsigned int p, unsigned int nu)
{
	int i = 0, base = (int) nu*n;
	uint64_t aux = 0;
	
	assert  (Spos != NULL);
	assert (XIpos != NULL);
	
	#pragma omp parallel for shared(Spos,XIpos) private(aux)
	for (i=0 ; i<n ; i++) {
		aux = mzran13() % 2;
		Spos[i] = aux > 0 ? XIpos[base+i] : XIpos[base];
	}
	
	return;
}



/* Avanza el estado de la red en 1 intervalo temporal, considerando que
 * cada elemento ocupa UNA POSICION de cada arreglo
 * Para ello actualiza una vez cada una de las neuronas en Spos,
 * y deja las nuevas superposiciones registradas en m
 *
 * PRE: Spos != NULL
 *	Spos es un vector de dimension [n]
 *	Spos[i] == 0 para todo 0 <= i < n
 *	XIpos != NULL
 *	XIpos es una matriz [p][n] (ie: con 'p' memorias de 'n' componentes c/u)
 *	m != NULL
 *	m es un vector de dimendion [p]
 */
static void
update_pos_net (long *Spos, long *XIpos, int *m,
		unsigned int n, unsigned int p)
{
	int i = 0, mu = 0;
	long oldSi = -1, tmp = 0;
	double h = 0.0;
	
	for (i=0 ; i<n ; i++) {
		h = 0.0;
		oldSi = S[i];
		
		/* Actualizamos la i-esima neurona */
		#pragma omp parallel for shared(XI,m) reduction(+:h)
		for (mu=0 ; mu<p ; mu++)
			h += (double) (XI[(mu*n)+i] * m[mu]);
		S[i] = norm (h);
		
		/* Actualizamos las superposiciones en m */
		tmp = S[i] - oldSi;
		if (S[i] != oldSi) {
			#pragma omp parallel for default(shared) private(mu)
			for (mu=0 ; mu<p ; mu++)
				m[mu] += XI[(mu*n)+i] * tmp;
		}
	}
	
	return;
}





int main (void)
{
	unsigned long *Sbit = NULL;	/* Estado con 1 neurona x bit */
	unsigned long *XIbit = NULL;	/* Memorias con 1 elemento x bit */
	long *Spos = NULL;		/* Estado con 1 neurona x posicion */
	long *XIpos = NULL;		/* Memorias con 1 elemento x posicion */
	int *m = NULL;			/* Superposicion memoria/estado */
	double start = 0.0, end = 0.0;	/* Para medir el tiempo de ejecucion */
	int i = 0, mu = 0, k = 0;
	double h = 0.0;
	
	
	/* Generamos los arreglos */
	Sbit = (unsigned long *) calloc (N/MOD, sizeof(unsigned long));
	assert (Sbit != NULL);
	
	XIbit = (unsigned long *) calloc (P*(N/MOD), sizeof(unsigned long));
	assert (XIbit != NULL);
	
	Spos = (long *) calloc (N, sizeof(long));
	assert (Spos != NULL);
	
	XIpos = (long *) calloc (P*N, sizeof(long));
	assert (XIpos != NULL);
	
	m = (int *) calloc (P, sizeof(int));
	assert (m != NULL);
	
	
	/* ### Bitwise version ### */
	start = omp_get_wtime ();
	
	init_XIbit (XIbit, P, N/MOD);
	init_Sbit (Sbit, N/MOD, XIbit, P, NU);
	
/**	Para debugging
*//*	printf ("XIbit[0][0]:\n");
	printbits ((uint64_t) XIbit[0]);
	printf ("XIbit[%d][0]:\n", P);
	printbits ((uint64_t) XIbit[(P-1)*(N/MOD)]);
	printf ("Sbit[0]:\n");
	printbits ((uint64_t) Sbit[0]);
*/	
	/** TODO: Realizar MAX_ITER ciclos de actualizacion */
	#pragma omp parallel for default(shared) private(mu,i)
	for (mu=0 ; mu<P ; mu++) {
		m[mu] = 0;
		for (i=0 ; i<(N/MOD))
	}
	
	end = omp_get_wtime ();
	printf ("\nBitwise logic: %f seg\n", end-start);
		
		
	
	/* ### Long version ### */
	start = omp_get_wtime ();
	
	init_XIpos (XIpos, P, N);
	init_Spos (Spos, N, XIpos, P, NU);
	
/**	Para debugging
*//*	k = 1;
	printf("\nXIpos[0][0]:\n");
	for (i=0 ; i<MOD ; i++) {
		printf ("%+ld", XIpos[i]);
		!(k++ % _byte_size) ? printf(" ") : 0;
	}
	k = 1;
	printf("\nXIpos[%d][0]:\n", P);
	for (i=0 ; i<MOD ; i++) {
		printf ("%+ld", XIpos[(P-1)*N+i]);
		!(k++ % _byte_size) ? printf(" ") : 0;
	}
	k = 1;
	printf("\nSpos[0]:\n");
	for (i=0 ; i<MOD ; i++) {
		printf ("%+ld", Spos[i]);
		!(k++ % _byte_size) ? printf(" ") : 0;
	}
*/	
	/** TODO: Realizar MAX_ITER ciclos de actualizacion */
	/* Inicializamos las superposiciones en cada m[mu] ... */
	#pragma omp parallel for default(shared) private(mu,i)
	for (mu=0 ; mu<P ; mu++) {
		m[mu] = 0;
		for (i=0 ; i<N ; i++)
			m[mu] += XIpos[(mu*N)+i] * Spos[i];
	}
	/* ... y arrancamos con el ciclo de actualizacion de la red */
	for (k=0 ; k < MAX_ITER ; k++)
		update_pos_net (Spos, XIpos, m, N, P);
	
	end = omp_get_wtime ();
	printf ("\nLong logic: %f seg\n", end-start);
	
	
	free (Sbit);	Sbit     = NULL;
	free (XIbit);	XIbit    = NULL;
	free (Spos);	Spos     = NULL;
	free (XIpos);	XIpos    = NULL;
	free (m);	m = NULL;
	
	return 0;
}
