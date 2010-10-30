#include <stdio.h>
#include "sdhn.h"

#define  n  68
#define  p  16
#define  k  4

#define  nu 8

int main (void)
{
	sdhn_t net = NULL;
	
	printf ("Make sure to compile with _DEBUG flag on\n");
	
	net = sdhn_create (n, p, k);
	
	sdhn_init_XI (net);
	sdhn_init_neigh (net);
	sdhn_init_w (net);
	sdhn_init_S (net, nu);
	
	net = sdhn_destroy (net);
	
	return 0;
}


