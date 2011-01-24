#include "rdg.h"


/* Default seed values */
unlong x=521288629, y=362436069, z=16163801, c=1, n=1131199209;

/* Main algorithm */
unlong mzran13 (void) {
	long int s;
	
	if (y>x+c) {
		s=y-(x+c);
		c=0;
	} else {
		s=y-(x+c)-18;
		c=1;
	}
	x=y;
	y=z;
	return (z=s) + (n=69069*n+1013904243);
}

/* Initialization routine (seed modification) */
void ran13set (unlong xx, unlong yy, unlong zz, long nn) {
	x=xx;
	y=yy;
	z=zz;
	n=nn;
	c=y>z;
}
