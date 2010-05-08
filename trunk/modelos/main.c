#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "mechanic.h"
#include "washingm.h"
#include "laundry.h"

/* Funcion que castea el parametro p de tipo INT
 * RETURNS:
 * 	< 0 	on error
 * 	>=0	cc
 */
static int get_iparam (int p, char **argv)
{
	int n = -1;
	char *err = NULL;
	
	n = (int) strtol (argv[p], &err, 10);
	if (err[0] != '\0') {
		printf("Error: en el argumento recibido: %s\n", argv[p]);
		exit (EXIT_FAILURE);
	}
	
	return n;
}


/* Funcion que castea el parametro p de tipo DOUBLE
 * RETURNS:
 * 	< 0 	on error
 * 	>=0	cc
 */
static double get_dparam(int p, char **argv)
{
	double n = -1;
	char *err = NULL;
	
	n = strtod(argv[p], &err);
	if (err[0] != '\0') {
		printf("Error: en el argumento recibido: %s\n", argv[p]);
		exit (EXIT_FAILURE);
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
	double	Tf = 0.0,	/* Tiempo medio de fallo de una lavadora */
		Tr = 0.0;	/* Tiempo medio de raparación de una lavadora */
	double	ftime = 0.0,	/* Tiempo de fallo de una simulación */
		ft    = 0.0,	/* Tiempo acumulado de fallos */
		ft2   = 0.0;	/* Para calcular varianza */
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
	N  = get_iparam (1, argv);
	S  = get_iparam (2, argv);
	M  = get_iparam (3, argv);
	Tf = get_dparam (4, argv);
	Tr = get_dparam (5, argv);
	Nsim = get_iparam (6, argv);
	
	printf ("Valores ingresados:\n"
			"N=%d\t\tS=%d\t\tM=%d\n"
			"Tf=%.6f\tTr=%.6f\tNSIM=%d\n",
		(int) N, (int) S, (int) M, Tf, Tr, (int) Nsim);
	
	/* Creamos la lavandería con todas sus lavadoras y mecánicos */
	laundry = laundry_create (N, S, M, Tf, Tr);
	out = fopen ("laundry_sim.out","w");
	
	/** ALGORITMO PRINCIPAL */
	for (i=0 ; i<Nsim ; i++) {
		
		/* Simulamos una vez */
		ftime = laundry_simulation (laundry);
		/* Acumulamos resultados */
		ft  += ftime;
		ft2 += ftime*ftime;
		/* Registramos resultados en archivo */
		buffSize = sprintf(auxBuf, "%.6f\n", ftime);
		fwrite (auxBuf, sizeof(char), buffSize, out);
	}
	/** FIN ALGORITMO PRINCIPAL */
	
	E  = ft/(double)Nsim;		/* E[X] = #(exitos) / #(experimentos) */
	V  = ft2/(double)Nsim - E*E;	/* V[X] = E[X^2] - (E[X])^2 */
	
	printf ("Tiempo medio de fallo del sistema y su desviación estándard\n"
		"para %d simulaciones corridas\nE[X] = %.6f\nDE[X] = %.6f\n",
		Nsim, E, sqrt(V));

	/* Limpiamos nuestras estructuras */
	laundry = laundry_destroy (laundry);
	fclose(out);

	return 0;
}
