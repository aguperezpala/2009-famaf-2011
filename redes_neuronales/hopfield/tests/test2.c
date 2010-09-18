#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define  N  (1<<13)
#define  P  (N/2)

int main (void)
{
	int *mem_i = NULL, *XI = NULL, *S = NULL;
	double *mem_d = NULL;
	int mu = 0, i = 0;
	clock_t start=0, end=0;
	
	
	/* Generamos los arreglos */
	mem_i = (int *) calloc (P, sizeof(int));
	assert (mem_i != NULL);
	
	mem_d = (double *) calloc (P, sizeof(double));
	assert (mem_d != NULL);
	
	XI = (int *) calloc (N, sizeof(int));
	assert (XI != NULL);
	
	S = (int *) calloc (N, sizeof(int));
	assert (S != NULL);
	
	
	/* Inicializamos el estado y las memorias */
	for (i=0 ; i<N ; i++)
		/** TODO: inicializar el estado 'S'
		 **	  inicializar las memorias 'XI'
		 **	  inicializar las superposiciones 'mem'
		 **/
		
		
		
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
	printf ("\nUsando m[mu] de tipo INT se tardo: %.4f seg\n",
		(double)end-start/(double)CLOCKS_PER_SEC);
		
	free (mem_i);	mem_i = NULL;
	free (mem_d);	mem_d = NULL;
	free (XI);	XI    = NULL;
	free (S);	S     = NULL;
	
	return 0;
}
