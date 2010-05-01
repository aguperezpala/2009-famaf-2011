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
	/** Parametros */
	unsigned int N = 0,	/* # total de maquinas operativas */
		     S = 0,	/* # total de maquinas de servicio */
		     M = 0,	/* # de mecanicos */
		     Tf = 0,	/* Tiempo medio de fallo de una lavadora */
		     Tr = 0,	/* Tiempo medio de raparación de una lavadora */
		     Nsim = 0;	/* # de simulaciones a correr */
	/** Variables internas */
	bool no_failure = true;	/* ¿El sistema aun es operativo? */
	unsigned long time = 0, failureTimes = 0; /* Tiempo  */
	unsigned int Nbroken = 0; /* # total de maquinas no operativas */
	
	
	parse_args (argc, argv, N, S, M, Tf, Tr, Nsim);
	
	/* Creamos los arreglos que contendran nuestras estructuras */
	op_machines	= create_machines (N);
	broken_machines = create_machines (N);
	serv_machines	= create_machines (S);
	m = create_mechanics (M);
	
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
			bring_to_op (serv_machines, op_machines)
			/* Llevamos al taller las maquinas recién rotas */
			give_to_mechanics (broken_machines, m);
			
			time++;
		}
	}
	
	return 0;
}
