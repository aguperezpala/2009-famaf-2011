#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mechanic.h"
#include "washingm.h"


int main (int argc, char **argv)
{
	laundry_t laundry = NULL;
	unsigned int N = 0,	/* # total de maquinas operativas */
		     S = 0,	/* # total de maquinas de servicio */
		     M = 0,	/* # de mecanicos */
		     Tf = 0,	/* Tiempo medio de fallo de una lavadora */
		     Tr = 0,	/* Tiempo medio de raparación de una lavadora */
		     Nsim = 0;	/* # de simulaciones a correr */
	int ftime = 0,	/* Tiempo de fallo de un experimento */
	    ft    = 0,	/* Tiempo acumulado de fallos */
	    ft2   = 0;	/* Para calcular varianza */
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
	N  = get_param (1, argv);
	S  = get_param (2, argv);
	M  = get_param (3, argv);
	Tf = get_param (4, argv);
	Tr = get_param (5, argv);
	Nsim = get_param (6, argv);
	
	/* Creamos la lavandería con todas sus lavadoras y mecánicos */
	laundry = create_laundry (N, S, M, Tf, Tr);
	out = fopen ("laundry_sim.out","w");
	
	/** ALGORITMO PRINCIPAL */
	for (i=0 ; i<Nsim ; i++) {
	
		/* Reinicializamos el experimento */
		reset_laundry (laundry);
		/* Iniciamos una nueva simulación */
		while ( !laundry_failure (laundry) ) {
			
			/* Hacer lo que haya que hacer este mes */
			wash_clothes (laundry);
			
			/* ¿El sistema dejo de ser operativo? */
			if ( !laundry_failure (laundry))
				laundry_increase_month (laundry);
		}
		/* Acumulamos el tiempo obtenido en este experimento */
		ft  += laundry_get_failure_time();
		ft2 += time*time;
	}
	/** FIN ALGORITMO PRINCIPAL */
	
	
	printf ("Tiempo medio de fallo del sistema y su desviación estándard "
		"para %d simulaciones\nE[X] = %.6f\nDE[X] = %.6f\n",
		Nsim, ft/(double)Nsim, ft2/(double);/** TODO TODO */

	/* Limpiamos nuestras estructuras */
	laundry = laundry_destroy (laundry);

	return 0;
}
