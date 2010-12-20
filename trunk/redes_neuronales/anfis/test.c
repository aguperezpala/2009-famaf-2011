#include <stdio.h>

#include "anfis.h"
#include "mzran13.h"

#define  N  4
#define  T  3




static void init_MF (MF a[N*T])
{
	int i = 0, j = 0;
	
	/* Construimos las funciones de membresía ... */
	for (i=0 ; i < T*N ; i++) {
		/* ... con cualquier función ... */
		a[i].k = i % MAX_FUNC;
		for (j=0 ; j < MAX_PARAM ; j++) {
			/* ... y parámetros cualesquiera */
			a[i].p[j] = ((double) mzran13()) / ((double) mzran13());
		}
	}
	
	return;
}



int main (void)
{
	anfis_t net = NULL;
	MF a[N*T];
	
	init_MF (a);
	
	net = anfis_create (N, T, (const MF *) a);
	
	anfis_print (net);
	
	net = anfis_destroy (net);
	
	return 0;
}
