#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "mechanic.h"
#include "washingm.h"
#include "laundry.h"

/* Funcion que castea el parametro p
* RETURNS:
* 	< 0 	on error
* 	>=0	cc
*/
static double get_param(int p, char **argv)
{
	double n = -1;
	char *err = NULL;
	
	n = strtod(argv[p], &err);
	if (err[0] != '\0') {
		printf("error en el argumento recibido: %s\n", argv[p]);
		return -1;
	}
	
	return n;
}



int main (int argc, char **argv)
{
	laundry_t laundry = NULL;
	unsigned int i = 0;
	unsigned int N = 0,	/* # total de maquinas operativas */
		     S = 0,	/* # total de maquinas de servicio */
		     M = 0,	/* # de mecanicos */		    
		     Nsim = 0;	/* # de simulaciones a correr */
	double  Tf = 0,		/* Tiempo medio de fallo de una lavadora */
		Tr = 0;		/* Tiempo medio de raparación de una lavadora */
	int ftime = 0,		/* Tiempo de fallo de un experimento */
	    ft    = 0,		/* Tiempo acumulado de fallos */
	    ft2   = 0;		/* Para calcular varianza */
	double E = 0.0, V = 0.0;	/* Esperanza y varianza */
	char auxBuf[15];
	int buffSize = 0;
	FILE *out = NULL;	/* Para registro de resultados */

	if (argc != 7) {
		fprintf (stderr, "Debe pasar 6 argumentos:\n"
				"1: # de maquinas operativas\n"
				"2: # de maquinas de servicio\n"
				"3: # de mecanicos\n"
				"4: Tiempo medio de fallo de una maquina\n"
				"5: Tiempo medio de reparacion\n"
				"6: # de simulaciones a correr\n");
		return -1;
	}

	/* Obtenemos los argumentos con que nos llamaron */
	N  = (unsigned int) get_param (1, argv);
	S  = (unsigned int) get_param (2, argv);
	M  = (unsigned int) get_param (3, argv);
	Tf = get_param (4, argv);
	Tr = get_param (5, argv);
	Nsim = (unsigned int) get_param (6, argv);
	
	/* Creamos la lavandería con todas sus lavadoras y mecánicos */
	laundry = laundry_create (N, S, M, Tf, Tr);
	out = fopen ("laundry_sim.out","w");
	
	/** ALGORITMO PRINCIPAL */
	for (i=0 ; i<Nsim ; i++) {
	
		/* Reinicializamos el experimento */
		laundry_reset (laundry);
		/* Iniciamos una nueva simulación */
		while ( !laundry_failure (laundry) ) {
			
			/* Hacer lo que haya que hacer este mes */
			laundry_wash_clothes (laundry);
			
			/* ¿El sistema dejo de ser operativo? */
			if ( !laundry_failure (laundry))
				laundry_increase_month (laundry);
		}
		ftime = laundry_get_failure_time(laundry); 
		/* Acumulamos el tiempo obtenido en este experimento */
		ft  += ftime;
		ft2 += ftime*ftime;
		
		/* ahora imprimimos en archivo el tiempo de falla */
		buffSize = sprintf(auxBuf, "%d\n", ftime);
		fwrite(auxBuf, 1, buffSize, out);
	}
	/** FIN ALGORITMO PRINCIPAL */
	
	E  = ft/ (double) Nsim;		/* E[X] = #(exitos) / #(experimentos) */
	V  = ft2/(double)Nsim - E*E;	/* V[X] = E[X^2] - (E[X])^2 */
	
	printf ("Tiempo medio de fallo del sistema y su desviación estándard\n"
		"para %d simulaciones corridas\nE[X] = %.6f\nDE[X] = %.6f\n",
		Nsim, E, sqrt(V));

	/* Limpiamos nuestras estructuras */
	laundry = laundry_destroy (laundry);
	fclose(out);

	return 0;
}
