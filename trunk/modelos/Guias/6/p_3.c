#include <stdio.h>
#include <math.h>
#include "rdg.h"
#include "ssv.h"

#define PAD	40	/* Definido para arquitectura de 64 bits */

unsigned int SIM = 1000;	/* # de simulaciones a realizar */

int main (void)
{
	/* Inicializacion de ran2 */
	struct timeval tv;
	long idum = 0;
	/* Variables relacionadas con las simulaciones */
	unsigned int i = 0, N = 0;
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	
	
	for (i=0 ; i<SIM ; i++) {
		N = 0;
		while (N < 1) {
			N = ran2
		}
	}
	
	return 0;
}
