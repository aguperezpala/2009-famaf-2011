#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mechanic.h"
#include "washingm.h"


int main (int argc, char **argv)
{
	/** Estructuras */
	wm_t *op_machines = NULL,	/* Maquinas operativas */
	     *serv_machines = NULL,	/* Maquinas de servicio */
	     *broken_machines = NULL;	/* Maquinas actualmente rotas */
	mechanic_t *m = NULL;		/* Mecanicos */
	int *broken_order = NULL;	/* Indices de orden */
	/** Parametros */
	unsigned int N = 0,	/* # total de maquinas operativas */
		     S = 0,	/* # total de maquinas de servicio */
		     M = 0,	/* # de mecanicos */
		     Tf = 0,	/* Tiempo medio de fallo de una lavadora */
		     Tr = 0,	/* Tiempo medio de raparación de una lavadora */
		     Nsim = 0;	/* # de simulaciones a correr */
	/** Variables internas */
	bool no_failure = true;	/* ¿El sistema aun es operativo? */
	unsigned long	time = 0,	/* Tiempo de ejecución de un experimento */
			ft   = 0;	/* Tiempo acumulado de fallos */
			ft2  = 0	/* Para calcular varianza */
	unsigned int Nbroken = 0; /* # total de maquinas no operativas */
	
	if (argc != 7) {
		fprintf (stderr, "Debe pasar 6 argumentos:\n"
			"1: # de maquinas operativas")
	}
	
	/* Obtenemos */
	N  = get_param (1, argv);
	S  = get_param (2, argv);
	M  = get_param (3, argv);
	Tf = get_param (4, argv);
	Tr = get_param (5, argv);
	Nsim = get_param (6, argv);
	
	/* Creamos los arreglos que contendran nuestras estructuras */
	op_machines	= create_machines (N);
	broken_machines = create_machines (N);
	serv_machines	= create_machines (S);
	m = create_mechanics (M);
	broken_order = (int *) calloc (N+1, sizeof(int));
	
	for (i=0 ; i<Nsim ; i++) {
		
		/* Reinicializamos el experimento */
		reset_laundry (op_machines, broken_machines, serv_machines, m);
		time = 0;
		no_failure = true;
		
		/** Iniciamos una nueva simulación */
		while (no_failure) {
			/* Obtenemos las máquinas que se acaban de romper
			 * y las sacamos de entre las operativas */
			Nbroken = get_broken (op_machines, time,
					      broken_machines, broken_order);
			
			/* Vemos cuantas máquinas hay en taller */
			Nbroken += get_repairing (m);
			
			/* ¿El sistema dejo de ser operativo? */
			if (Nbroken > S) {
				no_failure = false;
				continue;
			}
			
			/* Traemos a servicio las maquinas recién reparadas */
			get_from_mechanics (m, serv_machines);
			/* Reemplazamos los vacios con las maquinas de servicio */
			bring_to_operation (serv_machines, op_machines)
			/* Llevamos al taller las maquinas recién rotas */
			give_to_mechanics (broken_machines, m);
			
			time++;
		}
		/* Acumulamos el tiempo obtenido en este experimento */
		failureTimes += time;
	}
	
	return 0;
}
