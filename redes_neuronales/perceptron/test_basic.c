#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ptron_3l.h"


#define  NUM_LAYERS   NLAYERS
#define  INPUT_SIZE   5
#define  OUTPUT_SIZE  2

#define  ETHA       0.1
#define  ALPHA      0.9
#define  LBOUND    0.15
#define  UBOUND     0.3


const unsigned int N[NUM_LAYERS] = {INPUT_SIZE, 4, OUTPUT_SIZE};

double XI[INPUT_SIZE] = { -8.0,
			   5.02,
			   2.0,
			   4.0,
			   -0.996 };

double NU[OUTPUT_SIZE] = {0.906, 0.7107};


double f (double x);
double f (double x) { return sin(x); }


int main (void)
{
	ptron3_t net = NULL;
	ptron_dynamic mode = std;
	unsigned int NN[NUM_LAYERS] = {0, 0, 0};
	int i = 0, res = 0;
	double *O = NULL;
	double err = 0.0;
	
	/* Creation */
	net = ptron_create (N, ETHA, ALPHA, f, f);
	
	/* Getting layers' size info */
	ptron_get_layers_size (net, NN);
	for (i=0 ; i < NUM_LAYERS ; i++)
		printf ("N[%d] = %u\n", i, NN[i]);
	
	/* Restarting sinaptic weights */
	ptron_reinit_weights (net, LBOUND, UBOUND);
	
	/* Performing forward propagation of the input across the network */
	res = ptron_fwd_prop (net, XI, INPUT_SIZE);
	assert (res == PTRON_OK);
	
	/* Getting back the results obtained in the propagation */
	O = ptron_get_output (net);
	assert (O != NULL);
	printf ("Retrieved output:\n");
	for (i=0 ; i < OUTPUT_SIZE ; i++)
		printf ("O[%d] = %f\n", i, O[i]);
	free (O);
	O = NULL;
	
	/* Retrieving learning error for that propagation */
	err = ptron_calc_err (net, NU);
	printf ("Learning error generated: %.12f\n", err);
	
	/* Performing back-propagation for that input */
	mode = full;
	res = ptron_back_prop (net, NU, mode);
	assert (res == PTRON_OK);
	
	/* Clearing accumulated info */
	ptron_reset (net);
	
	/* Destruction */
	net = ptron_destroy (net);
	
	return 0;
}
