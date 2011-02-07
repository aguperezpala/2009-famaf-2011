#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "rdg.h"	/* Random Deviates Generators */


#define PAD 40	/* Definido para arquitectura de 64 bits */

int main1 (int argc, char * argv[]);
int main1 (int argc, char * argv[])
{
	unsigned int  i=0;
	unsigned long n=0;	/* Total de iteraciones a realizar */
	long idum = 0;		/* Para inicialización de ran2 */
	struct timeval tv;	/* Para inicialización de ran2 */
	double U1=0.0, U2=0.0;	/* v.a. 'U1' y 'U2' ~ U(0,1) */
	double F=0.0;		/* Fraccion de exitos obtenidos */
	char *err=NULL;
	clock_t start = 0, end = 0; /* Para medir el tiempo de cálculo */
	
	/* Manejo de la entrada */
	if (argc!=2) {
		fprintf (stderr, "Debe especificar N=\"nº de iteraciones del"
		" método\"\n");
		return -1;
	} else {
		/* Obtenemos el nº de iteraciones 'N' */
		n = (unsigned long) strtol (argv[1], &err, 10);
		if (err[0] != '\0') {
			printf ("Error en \'%s\'\n",err);
			fprintf (stderr, "Debe pasar N=\"nº de iteraciones del"
			" método\" como unico parámetro\n");
			return -1;
		}
	}
	
	/* Para inicializacion de ran2 */
	gettimeofday(&tv, NULL);
	idum = (long) -((tv.tv_sec << PAD) >> PAD);
	if (idum > 0)
		idum = -idum;
	
	/* Comenzamos a tirar numeros */
	F = 0.0;
	start = clock();
	for (i=0 ; i<n ; i++) {
		U1 = ran2(&idum);
		U2 = ran2(&idum);
		if ( ( U1<0.5 && U2>U1) || ( U1>0.5 && U2<U1 ) ) /* Exito */
			F += 1.0;
	}
	F = F / (double) n;
	end = clock();
	
	printf (">>> La fracció calculada de exitos es: %.12f\n",F);
	printf (">>> Tomó %.4f segundos de ejecución calcularla\n",
		(double) ((end-start+0.0)/CLOCKS_PER_SEC) );
	
	return 0;
}
