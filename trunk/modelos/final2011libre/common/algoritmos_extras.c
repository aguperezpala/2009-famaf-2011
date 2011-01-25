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
		/* swap */
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
double calc_prom(int n, int k, FUNC_PROM_PTR a)
{
	double result = 0.0;
	double U = 0.0;
	int i = 0;
	
	for(i = 0; i < k; i++) {
		U = disc_gen_uniforme(1,n);
		result += a(U);
	}
	
	result = result/(double)k;
	
	return result;
}


