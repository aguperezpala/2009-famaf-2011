#include <stdio.h>
#include <limits.h>
#include <inttypes.h> /* To have access to uuint64_t */
#include "../mzran13.h"


#define  _byte_size  (1<<3)
#define  MOD  (_byte_size*sizeof(unsigned long))


uint64_t mask[MOD];


static inline void
set_mask (void)
{
	mask[0] = 1;
	for (int i=1; i<MOD ; i++)
		mask[i] |= mask[i-1] | ((long unsigned)1)<<i;
}

/* Print n as a binary number */
static void
printbits (uint64_t n)
{
	uint64_t l = ((uint64_t)1) << 63;
	short k = 1;
	
	while (l > 0) {
		printf("%c", n&l ? '1' : '0');
		l >>= 1;
		!(k++ % _byte_size) ? printf(" ") : 0;
	}
	printf ("\n");
}


int main (void)
{
	long int i = 0;
	uint64_t aux = 0, tmp = 0;
	unsigned long S = 0;
	unsigned long XI = 0x0123456789ABCDEF;
	/* XI == (MSB) 0000 0001 0010 0011 0100 0101 0110 0111 ...
	 *	   ... 1000 1001 1010 1011 1100 1101 1110 1111 (LSB) */
	
/**	Example of how printbits works */
	set_mask();
	for (i=0 ; i<MOD ; i++) {
		printf ("mask[%ld]:\n", i);
		printbits(mask[i]);
	}

	
	for (i=0 ; i<MOD ; i++) {
		aux = mzran13();
		if (aux % 2)
			/* tmp = ξ(i) */
			tmp = ((unsigned long)1) << (MOD-1-i);
		else
			/* tmp = ξ(1) */
			tmp = ((unsigned long)1) << (MOD-1);
		tmp &= XI;
		S |= tmp;
	}
	printf ("XI = %lu:\n", XI);
	printbits ((uint64_t) XI);
	printf ("S  = %lu:\n", S);
	printbits (S);
	
	return 0;
}
