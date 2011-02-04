#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "rdg.h"
#include "gen_continuas.h"
#include "monte_carlo.h"
#include "algoritmos_extras.h"


#define TAM_MAZO	100
#define NUM_ITER	1000000



/* inicializa un arreglo de enteros de 0 a n-1 */
static void init_array(int *arr, int n)
{
	int i = 0;
	
	for(i = 0; i < n; i++){
		arr[i] = i;
	}
	
}


int main1(void)
{
	int mazo[TAM_MAZO];
	int i = 0, j = 0;
	int counter = 0;
	double esperanza = 0, esperanza2 = 0, var = 0, aux = 0;
	
	
	init_array(mazo, TAM_MAZO);
	
	for(i = 0; i < NUM_ITER; i++) {
		/* mezclamos */
		counter = 0;
		permutar_equiprobable(mazo,TAM_MAZO);
		
		for(j = 0; j < TAM_MAZO; j++){
			/* extraemos una carta */
			if(mazo[j] == j)
				counter++;
		}
		
		esperanza += (double)counter;
		esperanza2 += (double)counter*counter;
	}
	
	
	
	
	esperanza  = esperanza/ (double) NUM_ITER; /* E[X] = #(exitos) / #(experimentos) */
	esperanza2 = (esperanza2/(double)NUM_ITER);/* E[X^2] = #(exitos^2) / #(experimentos) */
	var  = esperanza2 - (esperanza*esperanza); /* V[X] = E[X^2] - (E[X])^2 */
	
	printf("E[X] = %f\tVar[X] = %f\n", esperanza, var);
	
	
	return 0;
}