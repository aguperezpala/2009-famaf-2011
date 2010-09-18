#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <inttypes.h> /* To have access to uuint64_t */
#include "../mzran13.h"

#define  _byte_size  (1<<3)
#define  PAD  (_byte_size*sizeof(size_t)-3)

#define  N  (1<<29)

/* Normalización a (-1,+1) (SIN BRANCHING)*/
#define  norm(x)  (((x) > 0) - ((x) < 1))
/* Normalización a (-1,+1) (CON BRANCHING)*/
#define  bnorm(x)  ((x) > 0 ? 1 : -1)


int main (void)
{
	long i = 0;
	clock_t start=0, end=0;
	struct timeval tv;
	uint64_t aux = 0;
	
	/* Inicializamos random */
	gettimeofday(&tv, NULL);
	srandom ((unsigned int) tv.tv_sec);
	
	
	/** ### LINUX RANDOM() ### */
	
	/** Caso: random() + PAD + branching */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (((uint64_t) random()) << PAD) >> PAD;
		aux = bnorm(aux);
/*		printf ("rand#%.2lu: %+i\n", i, (int) aux);
*/	}
	end = clock();
	printf ("\nCaso: random() + PAD + branching\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );
	
	
	/** Caso: random() + PAD */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (((uint64_t) random()) << PAD) >> PAD;
		aux = norm(aux);
	}
	end = clock();
	printf ("\nCaso: random() + PAD\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );

	
	/** Caso: random() + %2 + branching */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (uint64_t) random() % 2;
		aux = bnorm(aux);
/*		printf ("rand#%.2lu: %+i\n", i, (int) aux);
*/	}
	end = clock();
	printf ("\nCaso: random() + %%2 + branching\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );
	
	/** Caso: random() + %2 */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (uint64_t) random() % 2;
		aux = norm(aux);
	}
	end = clock();
	printf ("\nCaso: random() + %%2\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );
	
	
	
	/** ### MZRAN13() ### */
	
	/** Caso: mzran13() + PAD + branching */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (((uint64_t) random()) << PAD) >> PAD;
		aux = bnorm(aux);
	}
	end = clock();
	printf ("\nCaso: mzran13() + PAD + branching\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );
	
	
	/** Caso: mzran13() + PAD */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (((uint64_t) random()) << PAD) >> PAD;
		aux = norm(aux);
	}
	end = clock();
	printf ("\nCaso: mzran13() + PAD\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );

	
	/** Caso: mzran13() + %2 + branching */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (uint64_t) mzran13() % 2;
		aux = bnorm(aux);
	}
	end = clock();
	printf ("\nCaso: mzran13() + %%2 + branching\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );
	
	
	/** Caso: mzran13() + %2 */
	start = clock();
	for (i=0 ; i<N ; i++) {
		aux = (uint64_t) mzran13() % 2;
		aux = norm(aux);
	}
	end = clock();
	printf ("\nCaso: mzran13() + %%2\nTiempo total: %.3f seg\n",
		((end-start+0.0)/(double)CLOCKS_PER_SEC) );

	
	return 0;
}
