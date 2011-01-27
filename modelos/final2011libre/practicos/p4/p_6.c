#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#define ALFA 0.5
#define NUM_ITER	10000

double probX1(int i){	
	return pow(0.5, i);
}

double probX2(int i){	
	return (0.5*pow(2./3.,i));
}

int main(int argc, char *argv[]){
	int i = 0;
	double esperanza = 0.;
	double aux = 0;
	
	
	for(i = 0; i < NUM_ITER; i++){
		aux = disc_metodo_composicion(probX1, probX2, ALFA, 1);
		esperanza += aux;
		printf("aux: %f\n", aux);
	}	
	
	esperanza /= (double)NUM_ITER;
	printf("La esperanza con el metodo de composicion es: %f\n", esperanza);
	
	return 0;
}