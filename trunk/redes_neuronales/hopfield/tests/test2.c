#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <inttypes.h>
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
	int i = 0, j = 0;
	unsigned long tmp = 0;
	
	assert  (Sbit != NULL);
	assert (XIbit != NULL);
	
	#pragma omp parallel for default(shared) private(i,j,tmp)
	for (i=0 ; i<n ; i++) {
		for (j=0 ; j<MOD ; j++) {
			if (mzran13() % 2)
				/* tmp = ξ[nu][j] */
				tmp = ((unsigned long)1) << (MOD-1-j);
			else
				/* tmp = ξ[nu][1] */
				tmp = ((unsigned long)1) << (MOD-1);
			tmp &= XIbit[(nu*p)+i];
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
init_XIpos (unsigned long *XIpos, unsigned int p, unsigned int n)
{
	unsigned long i = 0;
	uint64_t aux = 0;
	
	assert (XIpos != NULL);
	
	/* NOTE El no determinismo del scheduler se escurre por aqui en la
	 * paralelizacion del ciclo, haciendo que las memorias NO se generen
	 * de manera identica entre llamadas sucesivas al programa.
	 *	Es decir: invocaciones consecutivas del programa resultaran
	 * en resultados de XI[] diferentes cada vez.
	 */
	#pragma omp parallel for shared(XIpos) private(aux)
	for (i=0 ; i<n*p ; i++) {
		aux = mzran13() % 2;
		XIpos[AT(i/n,i%n)] = norm(aux);
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
init_Spos (unsigned long *Spos , unsigned int n,
	   unsigned long *XIpos, unsigned int p, unsigned int nu)
{
	int i = 0;
	uint64_t aux = 0;
	
	assert  (Spos != NULL);
	assert (XIpos != NULL);
	
	#pragma omp parallel for shared(Spos,XIpos) private(aux)
	for (i=0 ; i<n ; i++) {
		aux = mzran13() % 2;
		Spos[i] = aux > 0 ? XIpos[(p*nu)+i] : XIpos[(p*nu)+0];
	}
	
	return;
}




int main (void)
{
	unsigned long *Sbit = NULL;	/* Estado con 1 neurona x bit */
	unsigned long *XIbit = NULL;	/* Memorias con 1 lugar x bit */
	unsigned long *Spos = NULL;	/* Estado con 1 neurona x posicion */
	unsigned long *XIpos = NULL;	/* Memorias con 1 lugar x posicion */
	int *m = NULL;		/* Superposicion memoria/estado */
	clock_t start=0, end=0;
	
	
	/* Generamos los arreglos */
	Sbit = (unsigned long *) calloc (N/MOD, sizeof(unsigned long));
	assert (Sbit != NULL);
	
	XIbit = (unsigned long *) calloc (P*(N/MOD), sizeof(unsigned long));
	assert (XIbit != NULL);
	
	Spos = (unsigned long *) calloc (N, sizeof(unsigned long));
	assert (Spos != NULL);
	
	XIpos = (unsigned long *) calloc (P*N, sizeof(unsigned long));
	assert (XIpos != NULL);
	
	m = (int *) calloc (P, sizeof(int));
	assert (m != NULL);
	
	
	/* ### Bitwise version ### */
	start = clock ();
	
	init_XIbit (XIbit, P, N/MOD);
	init_Sbit (Sbit, N/MOD, XIbit, P, NU);
	
	
	/** TODO: Realizar MAX_ITER ciclos de actualizacion */
	
	
	end = clock ();
	printf ("\nBitwise logic: %.4f seg\n",
		((double)end-start)/(double)CLOCKS_PER_SEC);
		
		
	
	/* ### Long version ### */
	start = clock ();
	
	init_XIpos (XIpos, P, N);
	init_Spos (Spos, N, XIpos, P, NU);
	
	
	/** TODO: Realizar MAX_ITER ciclos de actualizacion */
	
	
	end = clock ();
	printf ("\nLong logic: %.4f seg\n",
		((double)end-start)/(double)CLOCKS_PER_SEC);
	
	
	free (Sbit);	Sbit     = NULL;
	free (XIbit);	XIbit    = NULL;
	free (Spos);	Spos     = NULL;
	free (XIpos);	XIpos    = NULL;
	free (m);	m = NULL;
	
	return 0;
}
