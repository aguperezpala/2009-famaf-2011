#include <stdio.h>
#include <math.h>
#include "mzran13.h"
#include "ptron.h"

double	n1 = 1.2,
	n2 = 0.66,
	n3 = 1.51,
	n4 = 1.49;

#define  A  4
unsigned int N[A+1] = {5,4,3,5,1};

int main (void)
{
	ptron_t net = NULL;
	
	net = ptron_create (A, (const unsigned int *) N, tanh);
	
	ptron_reinit (net, n2, n1);
	
	net = ptron_destroy (net);
	
	return 0;
}
