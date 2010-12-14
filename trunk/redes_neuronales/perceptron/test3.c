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
#define  LBOUND    0.66
#define  UBOUND     1.2


const unsigned int N[NUM_LAYERS] = {INPUT_SIZE, 4, OUTPUT_SIZE};

double XI[INPUT_SIZE] = { -8.0,
			   5.02,
			   2000.0,
			   123579985.0,
			   0.996 };

double NU[OUTPUT_SIZE] = {12.006, 1.2107};


double f (double x);
double f (double x) { return cos(x); }


int main (void)
{
	ptron3_t net = NULL;
	unsigned int NN[NUM_LAYERS] = {0, 0, 0};
	int i = 0, res = 0;
	double *O = NULL;
	
	/* Creation */
	net = ptron_create (N, ETHA, f);
	
	/* Getting layers' size info */
	ptron_get_layers_size (net, NN);
	for (i=0 ; i < NUM_LAYERS ; i++)
		printf ("N[%d] = %u\n", i, NN[i]);
	
	/* Restarting sinaptic weights */
	ptron_reinit (net, LBOUND, UBOUND);
	
	
	O = O;
	res = res;
	/* Performing forward propagation of the input across the network 
	res = ptron_fwd_prop (net);
	assert (res == PTRON_OK);*/
	
	/* Getting back the results obtained in the propagation 
	O = ptron_get_output (net, O);
	assert (O != NULL);
	printf ("Retrieved output:\n");
	for (i=0 ; i < OUTPUT_SIZE ; i++)
		printf ("O[%d] = %f\n", i, O[i]);
	free (O);
	O = NULL;*/
	
	/* Performing back-propagation for that input */
	
	
	/* Destruction */
	net = ptron_destroy (net);
	
	return 0;
}
