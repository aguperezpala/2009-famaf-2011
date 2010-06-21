#include <stdio.h>
#include <assert.h>

/* # total de lineas en el archivo sample.dat */
#define  N	500

/* Mediana */
#define  MED	0.48863790
/* Valores maximo y minimo */
#define  ULIM	1.12633539
#define  LLIM	0.17165992
/* Whiskers maximo y minimo -- Se usan los cuartiles Q3 y Q1 resp. */
#define  UWHIS	0.61308287
#define  LWHIS	0.38729103

int main (void)
{
	unsigned int i = 0;
	double Xi = 0.0;
	FILE *sample = NULL;
	FILE *box = NULL;
	
	sample = fopen ("sample.dat","r");
	box = fopen ("box.dat","w");
	assert (sample != NULL);
	assert (box != NULL);
	
	for (i=0 ; i<N ; i++) {
		fscanf (sample, "%lf\n", &Xi);
		fprintf (box, "%.8f\t%.8f\t%.8f\t%.8f\t%.8f\t%.8f\n",
			 Xi, MED, ULIM, LLIM, UWHIS, LWHIS);
	}
	
	fclose (sample);
	fclose (box);
	
	return 0;
}
