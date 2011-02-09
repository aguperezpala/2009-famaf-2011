#include <stdio.h>
#include "random.h"
#include <math.h>
#include <stdlib.h>
#include "chi_cuadrado.h"

#define N 10
#define A 0
#define B 5

/* tenemos que calcular p = P(A < X < B)
 * Donde X tiene f(x) como distribucion, f(x) = (n-1) * pow(S,2)/pow(sigma,2).
 * Osea S2 = varianza muestral, sigma2 desconocido. entonces se utiliza
 * sigma2empirico para reemplazar por sigma2.
 */


double calcular_x_barra(double muestra[], int n){
	double x_barra = 0.;
	int i = 0;
	for (i=0; i < n; i++){
		x_barra += muestra[i];
	}
	return (x_barra / n);
}

double calcular_S_cuadrado(double muestra[], int n, double x_barra){
	int i = 0;
	double s_cuad = 0.;
	
	for (i=0; i < n; i++){
		s_cuad += pow((muestra[i] - x_barra), 2.);
	}
	return (s_cuad / (double)(n-1));
}

int main8(int argc, char *argv[]){
	double x_barra = 0., p = 0., s_cuad = 0., s_exp = 0., mu_empirico = 0.;
	double muestra_original[N] = {6.422, 7.968, 2.287, 5.679, 5.740, 7.254, 3.126, 3.443, 7.702, 5.680};
	double muestra_permutada[N];
	int cant_it = 0, j = 0, cant = 0;
	
	if (argc == 1){
		cant_it = 1;
	}else{
		cant_it = atoi(argv[1]);
	}	
	
	mu_empirico = calcular_x_barra(muestra_original, N); /* es igual al mu empirico */
	sigma2_empirico = /* calculamos sigma2 empirico = varianza empirica */;
	
	
	for(j=0;j<cant_it;j++){
		/* generamos una permutacion de la muestra original */
		permutar_con_rep(muestra_original, muestra_permutada, N);
		/* generamos media muestral de la permutacion */
		mediaMuestralPermutada = calcular_x_barra(muestra_permutada, N);
		/* calculamos varianza muestral para esa permutacion y ese x_barra */
		varianzaMuestralPermutada = ...;
		
		/* ahora generamos el valor con la f(x) de X  (f(x) = (n-1) * pow(S,2)/pow(sigma,2))*/
		p = (TAM_MUESTRA - 1) * varianzaMuestralPermutada/sigma2_empirico;
		
		/* verificamos si p esta dentro del rango P(A < X < B)*/
		if (A <= p && p <= B){
			cant++;
		}
	}
	
	/* luego calculamos p en base a la cantidad de exitos que obtuvimos */
	p = (double)cant/cant_it;
	
	printf("La estimacion de p usando bootstrap es: %f\n",p);
	
	return 0;
}
