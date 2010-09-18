#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <inttypes.h>
#include "../mzran13.h"

#define  _byte_size  (1<<3)
#define  MOD  (_byte_size*sizeof(long))

/* N y P deben ser multiplos de MOD */
#define  N  (1<<13)
#define  P  (N/2)
#define  NU  1


/* Normalización a (-1,+1) */
#define  norm(x)  ((x) > 0 ? 1 : -1)

/* Acceso a una matriz de dimensiones [P][N] */
#define  AT(i,j)  ((i)*P + (j)%N)



/* Inicializa aleatoriamente las p memorias en XI
 * PRE: XI != NULL
 */
static void init_mems (int *XI)
{
	long i = 0;
	uint64_t aux = 0;
	
	assert (XI != NULL);
	
	/* NOTE El no determinismo del scheduler se escurre por aqui en la
	* paralelizacion del ciclo, haciendo que las memorias NO se generen
	* de manera identica entre llamadas sucesivas al programa.
	*	Es decir: invocaciones consecutivas del programa resultaran
	* en resultados de XI[] diferentes cada vez.
	*/
	#pragma omp parallel for shared(XI)
	for (i=0 ; i<N*p ; i++) {
		aux = mzran13() % 2;
		XI[AT(i/N,i%N)] = norm(aux);
	}
	
	return;
}


/* Inicializa el estado 'S' en una posicion aleatoriamente cercana a XI[NU]
 * PRE: S != NULL
 */
static void init_state (int *S)
{
	int i = 0;
	uint64_t aux = 0;
	
	assert (S != NULL);
	
	#pragma omp parallel for shared(XI)
	for (i=0 ; i<N ; i++) {
		aux = mzran13() % 2;
		S[i] = aux > 0 ? XI[AT(NU,i)] : XI[AT(NU,1)];
	}
	
	return;
}




int main (void)
{
	long *Sbit = NULL;	/* Estado con 1 neurona x bit */
	long *Spos = NULL;	/* Estado con 1 neurona x posicion */
	long *XIbit = NULL;	/* Memorias con 1 lugar x bit */
	long *XIpos = NULL;	/* Memorias con 1 lugar x posicion */
	int *m = NULL;		/* Superposicion memoria/estado */
	clock_t start=0, end=0;
	
	
	/* Generamos los arreglos */
	Sbit = (long *) calloc (N/MOD, sizeof(long));
	assert (Sbit != NULL);
	
	Spos = (long *) calloc (N, sizeof(long));
	assert (Spos != NULL);
	
	XIbit = (long *) calloc (P*(N/MOD), sizeof(long));
	assert (XIbit != NULL);
	
	XIpos = (long *) calloc (P*N, sizeof(long));
	assert (XIpos != NULL);
	
	
	/* Inicializamos el estado y las memorias */
	/** TODO
	 **	1) Generar rutinas de inicializacion para los arreglos *bit
	 **	2) Reutilizar init_mems e init_state para los arreglos *pos
	 **	3) Inicializar los arreglos *bit y *pos
	 **	4) Realizar MAX_ITER ciclos de actualizacion en cada caso
	 **	5) Imprimir el (tiempo medido / MAX_ITER) en ambos casos
	 **/
	init_mems (XI);
	init_state (S);	
		
		
	/* Prueba con enteros */
	start = clock ();
	for (mu=0 ; mu<P ; mu++) {
		for (i=0 ; i<N ; i++)
			mem_i[mu] += XI[mu] * S[i];
	}
	end = clock ();
	printf ("\nUsando m[mu] de tipo INT se tardo: %.4f seg\n",
		(double)end-start/(double)CLOCKS_PER_SEC);
	
	
	/* Prueba con doubles */
	start = clock ();
	for (mu=0 ; mu<P ; mu++) {
		mem_d[mu] += XI[mu] * S[i];
	}
	end = clock ();
	printf ("\nUsando m[mu] de tipo DOUBLE se tardo: %.4f seg\n",
		(double)end-start/(double)CLOCKS_PER_SEC);
		
	free (mem_i);	mem_i = NULL;
	free (mem_d);	mem_d = NULL;
	free (XI);	XI    = NULL;
	free (S);	S     = NULL;
	
	return 0;
}
