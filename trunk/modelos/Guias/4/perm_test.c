#include <stdio.h>
#include "permuter.h"

#define N 20

static void init_int_array (int *a, unsigned int n)
{
	unsigned int i=0;
	for (i=0 ; i<n ; i++) {
		a[i] = (int) i;
	}
	return;
}


int test (void);

int test (void)
{
	int a[N], i=0;
	
	/* Inicializamos el arreglo */
	init_int_array ((int *) a, N);
	
	printf ("Arreglo inicial: [");
	for (i=0 ; i<N ; i++){
		printf ("%d, ",a[i]);
	}
	printf ("]\n");
	
	/* Lo permutamos aleatoriamente */
	perm_int (a, N);
	
	printf ("Arreglo permutado: [");
	for (i=0 ; i<N ; i++){
		printf ("%d, ",a[i]);
	}
	printf ("]\n");
	
	return 0;
}
