#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

#include "anfis.h"
#include "mzran13.h"

#define  N  4
#define  T  3




static void init_MF (MF_t mf[N*T])
{
	int i = 0, j = 0;
	
	/* Construimos las funciones de membresía ... */
	for (i=0 ; i < T*N ; i++) {
		/* ... con cualquier función ... */
		mf[i].k = i % MAX_FUNC;
		for (j=0 ; j < MAX_PARAM ; j++) {
			/* ... y parámetros cualesquiera */
			mf[i].p[j] = (double) i+j;
		}
	}
	
	return;
}




static void print_MF (MF_t mf)
{
	char MF_kinds[MAX_FUNC][25] = {"triang", "trap", "gauss", "bell"};
	
	printf ("MF type: %s\nParameters: ", MF_kinds[mf.k]);

	switch (mf.k) {
	
	case (triang):
		printf ("a = %.4f, b = %.4f, c = %.4f\n", 
			mf.p[0],
			mf.p[1],
			mf.p[2] );
		break;
		
	case (trap):
		printf ("a = %.4f, b = %.4f, c = %.4f, d = %.4f\n", 
			mf.p[0],
			mf.p[1],
			mf.p[2],
			mf.p[3] );
		break;
		
	case (gauss):
		printf ("a = %.4f, σ = %.4f\n", 
			mf.p[0],
			mf.p[1] );
		break;
		
	case (bell):
		printf ("a = %.4f, b = %.4f, c = %.4f\n", 
			mf.p[0],
			mf.p[1],
			mf.p[2] );
		break;
		
	default:
		printf ("Error: unknown membership function\n");
		break;
	}
}




int main (void)
{
	anfis_t net = NULL;
	MF_t mf[N*T];
	double *p = NULL;
	int res = 0, i = 0;
	
	
	init_MF (mf);
	
	printf ("Creating sample ANFIS network\n\n");
	net = anfis_create (N, T, (const MF_t *) mf);
	anfis_print (net);
	
	
	/* Getting a membership function */
	printf ("Retrieving membership function...\n");
	res = anfis_get_MF (net, T-1, N-1, &(mf[0]));
	assert (res == ANFIS_OK);
	print_MF (mf[0]);
	printf ("Should be equal to the last MF in the network\n");
	
	
	/* Setting a membership function */
	printf ("\nCreating dummy MF...\n");
	mf[0].k = gauss;
	mf[0].p[0] = 1.01010101;
	mf[0].p[1] = 9.87654321;
	print_MF (mf[0]);
	printf ("Setting MF as the network's first (ie: Branch # 0 / MF[0])");
	res = anfis_set_MF (net, 0, 0, mf[0]);
	assert (res == ANFIS_OK);
	anfis_print_branch (net, 0);
	
	
	/* Getting consequent parameters */
	printf ("\n\nRetrieving last consequent parameters...\n");
	p = anfis_get_P (net, T-1);
	assert (p != NULL);
	printf ("p =");
	for (i=0 ; i <= N ; i++)
		printf ("  %.2f", p[i]);
	
	
	/* Getting consequent parameters */
	printf ("\n\nCreating dummy consequent parameters...\np =");
	for (i=0 ; i <= N ; i++) {
		p[i] = ((double) mzran13()) / ((double) ULONG_MAX) + ((double) N-i);
		printf ("  %.2f", p[i]);
	}
	printf ("\nSetting p as the first consequent parameters (ie: Branch # 0)");
	res = anfis_set_P (net, 0, p);
	assert (res == ANFIS_OK);
	anfis_print_branch (net, 0);
	free (p);	p = NULL;
	
	
	net = anfis_destroy (net);
	
	return 0;
}
