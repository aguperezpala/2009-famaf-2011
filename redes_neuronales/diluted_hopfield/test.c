#include <stdio.h>
#include "sdhn.h"
#include <math.h>

#define  UNTRACED  0
#define  TRACED  10

#define  n  64
#define  p  2
#define  k  3

#define  nu 0

int main (void)
{
	sdhn_t net = NULL;
	
	printf ("Make sure to compile with _DEBUG flag on\n");
	
	net = sdhn_create (n, p, k);
	
	sdhn_set_traced   (net, TRACED);
	sdhn_set_untraced (net, UNTRACED);
	
	sdhn_init_XI (net);
	sdhn_init_neigh (net);
	sdhn_init_w (net);
	sdhn_init_S (net, nu);
	
	sdhn_run_net (net, nu);
	
	net = sdhn_destroy (net);
	
	return 0;
}


