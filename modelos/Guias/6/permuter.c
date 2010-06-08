#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "rdg.h"
#include "permuter.h"


#define PAD 40	/* Definido para arquitectura de 64 bits */


/* Devuelve la permutacion aleatoria de un arreglo "a" de longitud "n" cuyos
 * elementos ocupan "size" bytes en memoria
 * Si el arreglo tiene punteros a otros bloques, no se copiaran los bloques en
 * si sino sus direcciones de memoria.
 * PRE: length(a) <= n
 *	"a tiene al menos un elemento, en la posicion 0"
 * POS: "los elementos de a fueron permutados aleatoriamente"
 * FIXME: NO FUNCIONA !!!
 */
void perm (void **a, unsigned int n, size_t size)
{
	unsigned int nn=0, I=0;
	void *tmp=NULL;
	
	assert (a != NULL);
	tmp = malloc (size);
	assert (tmp != NULL);
	
	for (nn=n-1 ; nn>0 ; nn--) {
		I = (unsigned int ) (mzran13()>>7) % nn;
		memcpy (tmp, *(a+(I*size)), size);
		memcpy (*(a+(I*size)), *(a+(nn*size)), size);
		memcpy (*(a+(nn*size)), tmp,  size);
	}
	
	free(tmp);
	tmp = NULL;
	
	return;
}


/* Devuelve la permutacion aleatoria de un arreglo de ints "a" de longitud "n"
 * PRE: 0 < length(a) < n
 * POS: "los elementos de a fueron permutados aleatoriamente"
 */
void perm_int (int *a, unsigned int n)
{
	unsigned int nn=0, I=0;
	struct timeval tv;	/* Para inicialización de ran2 */
	long idum=0;
	
	assert (a != NULL);
	
	/* Inicializamos ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	for (nn=n-1 ; n>0 ; n--) {
		I = mzran13() % nn;
/*		I = floor ((double) ran2(&idum) * nn);*/
		/* Swap entre a[I] y a[nn] */
		a[I] += a[nn];
		a[nn] = a[I] - a[nn];
		a[I] -= a[nn];
	}
	
	return;
}

