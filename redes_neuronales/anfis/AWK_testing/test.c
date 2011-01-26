#include <stdio.h>
#include <limits.h>
#include "../mzran13.h"

/* NOTE test.sh depends on this file */

#define  T	7
#define  N	5
#define  width	15
#define  ran()	(((double) mzran13()) / ((double) ULONG_MAX))

int main (void)
{
	int i = 0, j = 0;
	
	for (i=0 ; i < T ; i++) {
		for (j=0 ; j < N ; j++) {
			printf ( "%d%*d\t%-*.4f%-*.4f%-*.4f\n",
				 i, 7, j,
				 width, i*ran()+j,
				 width, i*i*ran()+j+j,
				 width, i*i*i*ran()+j+j+j );
		}
	}
	
	return 0;
}