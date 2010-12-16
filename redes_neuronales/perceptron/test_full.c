#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#include "mzran13.h"
#include "ptron_3l.h"


#define  NUM_LAYERS   NLAYERS
#define  INPUT_SIZE   12
#define  OUTPUT_SIZE  3

#define  ETHA       0.1
#define  ALPHA      0.9
#define  LBOUND     0.0
#define  UBOUND     1.0

#define  MAX_ITER    10

const unsigned int N[NUM_LAYERS] = {INPUT_SIZE, 8, OUTPUT_SIZE};

double XI[INPUT_SIZE];
double NU[OUTPUT_SIZE];

double f (double x);
double f (double x) { return sin(x); }


static void
gen_example (void)
{
	int j = 0;
	
	for (j=0 ; j < OUTPUT_SIZE ; j++)
		NU[j] = 0.0;
	for (j=0 ; j < INPUT_SIZE ; j++) {
		XI[j] = ((double) mzran13()) / ((double) ULONG_MAX);
		if (XI[j % OUTPUT_SIZE] > 0.5)
			NU[j % OUTPUT_SIZE] += 0.25;
	}
	return;
}


int main (void)
{
	ptron3_t net = NULL;
	ptron_dynamic mode = std;
	int i = 0, res = 0;
	
	net = ptron_create (N, ETHA, ALPHA, f, f);
	
	mode = full;
	
	for (i=0 ; i < MAX_ITER ; i++) {
		
		fprintf (stderr, "\n\n\t\t\t\t\t\t\t\tIteration #%d\n", i);
		
		/* Restarting sinaptic weights */
		ptron_reinit_weights (net, LBOUND, UBOUND);
		
		/* Generating new training set */
		gen_example();
		
		/* Performing forward propagation of the input */
		res = ptron_fwd_prop (net, XI, INPUT_SIZE);
		assert (res == PTRON_OK);
		
		/* Performing back-propagation for that input */
		res = ptron_back_prop (net, NU, mode);
		assert (res == PTRON_OK);
	}
	
	/* Destruction */
	ptron_reset (net);
	net = ptron_destroy (net);
	
	return 0;
}
