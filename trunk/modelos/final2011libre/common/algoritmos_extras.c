#include "algoritmos_extras.h"


/******************************************************************************/
void permutar_equiprobable(int *arr, int size)
{
	int k = 0;
	int p = 0;
	int aux = 0;
	
	k = size-1;
	while(k > 0){
		p = disc_gen_uniforme(0,k);
		aux = arr[k];
		arr[k] = arr[p];
		arr[p] = aux;
		k--;
	}
}

/******************************************************************************/
void permutar_no_equiprobable(int *arr, int size)
{
	int k = 0;
	int p = 0;
	int aux = 0;
	
	k = size-1;
	while(k > 0){
		p = disc_gen_uniforme(0,size-1);
		/* swap */
		aux = arr[k];
		arr[k] = arr[p];
		arr[p] = aux;
		k--;
	}
	
}

/******************************************************************************/
void permutar_con_repeticion(double *original, double *permutado, int tam)
{
	int i = 0, j = 0;
	for(j=0;j<tam;j++){
		i = disc_gen_uniforme(0,tam);
		permutado[j] = original[i];
	}
}


/******************************************************************************/
double calc_prom(int n, int k, FUNC_PROM_PTR func)
{
	double result = 0.0;
	double U = 0.0;
	int i = 0;
	
	for(i = 0; i < k; i++) {
		U = disc_gen_uniforme(1,n);
		result += func(U);
	}
	result = result*((double)k);
	
	return result;
}

/******************************************************************************/
void sort_doubles(double *arr, int size) 
{
	int a = 0, b = 0;
	double t = 0;
	
	for(a = 1; a < size; ++a)
		for(b = size-1; b >= a; --b) {
			if(arr[ b - 1] > arr[ b ]) {
				t = arr[ b - 1];
				arr[ b - 1] = arr[ b ];
				arr[ b ] = t;
			}
		}
}
