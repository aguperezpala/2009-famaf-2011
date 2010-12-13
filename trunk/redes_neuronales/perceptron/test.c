#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "mzran13.h"
#include "ptron.h"


#define  LBOUND    0.66
#define  UBOUND     1.2
#define  A  4
#define  INPUT_SIZE   5
#define  OUTPUT_SIZE  2

unsigned int N[A+1] = {INPUT_SIZE, 4, 3, 5, OUTPUT_SIZE};

double XI[INPUT_SIZE] = { -8.0,
			   5.02,
			   2000.0,
			   123579985.0,
			   0.996 };



int main (void)
{
	ptron_t net = NULL;
	unsigned int *NN = NULL;
	int i = 0, res = 0;
	double *O = NULL;
	
	/* Creation */
	net = ptron_create (A, (const unsigned int *) N, tanh);
	
	/* Getting layers' size info */
	NN = ptron_get_layers_size (net, NN);
	for (i=0 ; i <= A ; i++)
		printf ("N[%d] = %u\n", i, NN[i]);
	free (NN);
	
	/* Restarting sinaptic weights */
	ptron_reinit (net, LBOUND, UBOUND);
	
	/* Setting input */
	printf ("Setting new input:\n");
	for (i=0 ; i < INPUT_SIZE ; i++)
		printf ("XI[%d] = %.3f\n", i, XI[i]);
	ptron_set_input (net, (const void *) XI, INPUT_SIZE);
	
	/* Performing forward propagation of the input across the network */
	res = ptron_fwd_prop (net);
	assert (res == PTRON_OK);
	
	/* Getting back the results obtained in the propagation */
	O = ptron_get_output (net, O);
	assert (O != NULL);
	printf ("Retrieved output:\n");
	for (i=0 ; i < OUTPUT_SIZE ; i++)
		printf ("O[%d] = %f\n", i, O[i]);
	free (O);
	O = NULL;
	
	/* Destruction */
	net = ptron_destroy (net);
	
	return 0;
}
