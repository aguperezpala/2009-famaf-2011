#include <stdio.h>
#include <stdlib.h>

#define N 10

static int cmp_dbl (const void *a, const void *b)
{
	int res = 0;

	res  =  (*((const double *) a) > *((const double *) b)) ?  1 : \
		(*((const double *) a) < *((const double *) b)  ? -1 : 0);
	
	return res;
}

int main (void)
{
	double a = 1.0, b = 0.0;
	double sample[10] = {1.123, 0.11, -4.89, -200.0, 100.0,
			     1.0, 2.33, 1.0, -33.0, 55.0 };
	int i = 0;
	
	printf ("cmp_dbl (1, 0) == %d\n"
		"cmp_dbl (0, 1) == %d\n"
		"cmp_dbl (0, 0) == %d\n"
		"cmp_dbl (1, 1) == %d\n",
		cmp_dbl ((const void *) &a, (const void *) &b),
		cmp_dbl ((const void *) &b, (const void *) &a),
		cmp_dbl ((const void *) &b, (const void *) &b),
		cmp_dbl ((const void *) &a, (const void *) &a)  );
	
	
	printf ("\nsample:");
	for (i=0 ; i<N ; i++) printf (" %.1f", sample[i]);
	
	
	/* Ordenamos la muestra en orden creciente */
	qsort (sample, (size_t) N, sizeof (double), cmp_dbl);
	printf ("\n\nSORTED sample");
	for (i=0 ; i<N ; i++) printf (" %.1f", sample[i]);
	printf ("\n\n");
	
	return 0;
}

