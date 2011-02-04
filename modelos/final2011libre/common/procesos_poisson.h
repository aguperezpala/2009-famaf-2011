#ifndef PROCESOS_POISSON_H
#define PROCESOS_POISSON_H

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

typedef double(*func_ptr)(double);

/* Generacion de proceso de poisson homogeneo
* cada evento ei ~ Exp(lambda). entonces para calcular cuantos eventos
* ocurrieron entre [0,T] y sus respectivos tiempos de ocurrencia hacemos:
* NOTE: devuelve en timesResults los tiempos ordenados de los eventos,
*  	 el tamaño del arreglo = al valor devuelto por la funcion (=cant de 
*	 eventos ocurridos entre 0 y T). Los eventos empiezan en 1
* NOTE: debe ser allocado anteriormente
*/
int process_poisson_homogeneo(double T, double lambda, double *timesResults);


/* Proceso de poisson no homogeno, tenemos la funcion lambda(t).. debemos
* buscar un lambda tal que lambda(t) <= lambda, luego mientras mas cerca este
* lambda de lambda(t) mas rapido es el algoritmo
* PARAMS:
*	T = hasta el tiempo T.
*	funLambda	= funcion lambda del proceso (determina frecuencia de arribos)
* 	lambda = constante tal que funLambda(t) <= lambda para todo t <= T.
*	timeResults = resultados de los tiempos de arribo.
*/
int process_poisson_no_homogeneo(double T, func_ptr funLambda, double lambda, 
				 double *timesResults);

/* processo de poisson no homogeneo por adelgazamiento o muestreo aleatorio.
* particionar el intervalo (0,T) en subintervalos y aplicar el algoritmo
* en c/u de estos subintervalos.
* 0 = t0 < t1 < · · · < tk < tk +1 = T, con lambda(s) <= lambdaj, con
* j-1 <= s < j.
* Pasamos en lambdas el arreglo de los lambdasj (Comenzando en lambda = 0)
* en ts pasamos los tj
* k = tamaño del los ts -1 --- (ts[k+1] = T)
* funLambda	= funcion lambda del proceso (determina frecuencia de arribos)
*/
int process_poisson_no_homogeneo_mejorado(double *ts, int k, double *lambdas, 
				 double *timesResults, func_ptr funLambda);


/* Generacion de eventos para un processo de poisson no homogeneo utilizando
* el metodo de la transformada inversa 

void gen_evnts_poisson_no_homogeneo(double *tRes, double T)
{
	int i = 0;
	double U = 0;
	tRes[0] = 0;
	
	while (1) {
		U = (double)mzran13()/(double)ULONG_MAX;
		if (((tRes[i] + 3*U)/(double)(1-U)) > T) 
			return;
		i++;
		tRes[i] = (tRes[i-1] + 3*U)/(1-U);
	}
}
*/



#endif
