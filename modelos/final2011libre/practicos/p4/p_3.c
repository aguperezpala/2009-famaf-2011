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


#define NUM_ITER		100000

/* el mapa de bits va de 2 a TAM_MAPA-1 */
#define TAM_MAPA	13
static int mapaBits[TAM_MAPA] = {};
static int numBitsSetted = 0;

/* Usamos un arreglo para representar el mapa de bits */
static void reset_mapa_bits(void)
{
	int i = 0;
	for(i = 0; i < TAM_MAPA; i++)
		mapaBits[i] = 0;
	
	/* Iniciamos para este caso especial el 0,1, en 1 */
	mapaBits[0] = mapaBits[1] = 1;
	numBitsSetted = 2;
}

static void set_bit(int pos)
{
	if(mapaBits[pos] == 1)
		return;
	
	mapaBits[pos] = 1;
	numBitsSetted++;
}

static void reset_bit(int pos)
{
	if(mapaBits[pos] == 0)
		return;
	
	mapaBits[pos] = 0;
	numBitsSetted--;
}

static int is_setted(int pos)
{
	return mapaBits[pos];
}

static int all_bits_setted()
{
	return (numBitsSetted == TAM_MAPA);
}

/* Tirar dados */
int tirar_dados(void)
{
	int dado1 = disc_gen_uniforme(1,7);
	int dado2 = disc_gen_uniforme(1,7);
	
	return dado1+dado2;	
}


int main(void)
{
	int iter = 0;
	int lanzamientos = 0;
	double esperanza = 0, esperanza2 = 0, varianza;
	int dados = 0;
	
	for(iter = 0; iter < NUM_ITER; iter++) {
		reset_mapa_bits();
		lanzamientos = 0;
		
		while(!all_bits_setted()) {
			dados = tirar_dados();
			set_bit(dados);
			lanzamientos++;
		}
		
		esperanza += (double) lanzamientos;
		esperanza2 += (double) lanzamientos*lanzamientos;
	}
	
	esperanza /= (double) NUM_ITER;
	esperanza2 /= (double) NUM_ITER;
	varianza = esperanza2 - esperanza*esperanza;
	
	printf("N: %d\tE[X]: %f\tDesv.estandard[X]: %f\n", NUM_ITER, esperanza,
	       sqrt(varianza));
	
	
	return 0;
}