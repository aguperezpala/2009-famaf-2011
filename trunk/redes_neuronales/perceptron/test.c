#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mzran13.h"
#include "ptron.h"

double	n1 = 1.2,
	n2 = 0.66,
	n3 = 1.51,
	n4 = 1.49;

#define  A  4
#define  INPUT_SIZE  5

unsigned int N[A+1] = {INPUT_SIZE, 4, 3, 5, 1};
int XI[INPUT_SIZE] = {-8, 5, 2000, 123579985, 0};

int main (void)
{
	ptron_t net = NULL;
	unsigned int *NN = NULL;
	int i = 0;
	
	net = ptron_create (A, (const unsigned int *) N, tanh);
	
	ptron_reinit (net, n2, n1);
	
	NN = ptron_get_layers_size (net, NN);
	for (i=0 ; i <= A ; i++)
		printf ("N[%d] = %u\n", i, NN[i]);
	free (NN);
	
	ptron_set_input (net, (const void *) XI, INPUT_SIZE, sizeof(int));
	
	net = ptron_destroy (net);
	
	return 0;
}
