#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mechanic.h"
#include "washingm.h"



#define INV assert(l!=NULL);



struct _laundry {
	/* Máquinas lavadoras */
	wm_t *all_machines;	/* Todas las máquinas */
	wm_t *op_machines;	/* Referencia a las maquinas operativas */
	wm_t *serv_machines;	/* Referencia a las maquinas de servicio */
	wm_t *broken_machines;	/* Referencia a las maquinas actualmente rotas */
	int *broken_order;	/* Indices de orden */
	/* Mecanicos */
	mechanic_t *m;		/* Mecanicos */
	/* Cantidades */
	unsigned int N; 	/* # total de maquinas operativas */
	unsigned int S; 	/* # total de maquinas de servicio */
	unsigned int M; 	/* # de mecanicos */
	unsigned int Tf;	/* Tiempo medio de fallo de una lavadora */
	unsigned int Tr;	/* Tiempo medio de raparación de una lavadora */
	int time;		/* Tiempo de operación de la lavandería */
}


/* Traemos a servicio las maquinas recién reparadas */
static void get_from_mechanics (laundry_t *l)
{
	int i = 0;
	
	INV
	
	for (i=1 ; i<l->M ; i++) {
		
}
/* Llevamos al taller las maquinas recién rotas */
static void give_to_mechanics (laundry_t *l);
/* Reemplazamos los vacios con las maquinas de servicio */
static void bring_to_operation (laundry_t *l);
			


void wash_clothes (laundry_t *l) {
	
	/* Traemos a servicio las maquinas recién reparadas */
	get_from_mechanics (m, serv_machines);
	/* Llevamos al taller las maquinas recién rotas */
	give_to_mechanics (broken_machines, m);
	/* Reemplazamos los vacios con las maquinas de servicio */
	bring_to_operation (serv_machines, op_machines, (int) time);
			
}


/* Se fija en el total de máquinas que están siendo reparadas
 * ie: las que están en todos los mecánicos, y sus colas de reparación
 */
bool laundry_failure (laundry) {
	return true;
}


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
			ft   = 0,	/* Tiempo acumulado de fallos */
			ft2  = 0;	/* Para calcular varianza */
	unsigned int Nbroken = 0; /* # total de maquinas no operativas */
	/** Registro de resultados */
	FILE *out = NULL;
	
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
	
	/* Creamos los arreglos que contendran nuestras estructuras */
	op_machines	= create_machines (N);
	serv_machines	= create_machines (S);
	broken_machines = malloc (N);
	m = create_mechanics (M);
	/* Para facilitar la reinicialización de una simulac. crearemos un... */
	
	/* Todas las máquinas de la lavandería */
	all_machines	= create_machines (N+S);
	/* Máquinas en operación */
	op_machines	= (wm_t *) malloc (N * sizeof(struct wm_t));
	/* Máquinas en servicio (disponibles) */
	serv_machines	= (wm_t *) malloc (S * sizeof(struct wm_t));
	/* Máquinas que se acaban de romper al inicio de un mes */
	broken_machines	= (wm_t *) malloc (N * sizeof(struct wm_t));
	/* Indices que indican la posicion de las maquinas rotas */
	broken_order = (int *) calloc (N, sizeof(int));
	
	out = fopen ("laundry_sim.out","w");
	
	for (i=0 ; i<Nsim ; i++) {
		
		/* Reinicializamos el experimento */
		reset_laundry (all_machines, op_machines, N, serv_machines, S, m);
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
			/* Llevamos al taller las maquinas recién rotas */
			give_to_mechanics (broken_machines, m);
			/* Reemplazamos los vacios con las maquinas de servicio */
			bring_to_operation (serv_machines, op_machines, (int) time);
			
			time++;
		}
		/* Acumulamos el tiempo obtenido en este experimento */
		ft  += time;
		ft2 += time*time;
	}
	
	printf ("Tiempo medio de fallo del sistema y su desviación estándard "
	       "para %d simulaciones\nE[X] = %.6f\nDE[X] = %.6f\n",
	       ft/(double));/** TODO TODO */
	
	/* Limpiamos nuestras estructuras */
	all_machines = destroy_machines (all_machines);
	m = destroy_mechanics (m);
	free (op_machines); op_machines = NULL;
	free (serv_machines); serv_machines = NULL;
	free (broken_machines); broken_machines= NULL;
	free (broken_order); broken_order = NULL;
	fclose (out);
	
	return 0;
}
