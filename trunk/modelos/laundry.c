#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "mechanic.h"
#include "washingm.h"
#include "laundry_aux.h"



#define INV assert(l != NULL); \
	    assert(l->all_machines != NULL); \
	    assert(l->op_machines != NULL); \
	    assert(l->serv_machines != NULL); \
	    assert(l->m != NULL); \
	    assert(!l->failure);



struct _laundry {
	/* Máquinas lavadoras */
	wm_t *all_machines;	/* Todas las máquinas */
	wm_t *op_machines;	/* Referencia a las maquinas operativas */
	wm_t *serv_machines;	/* Referencia a las maquinas de servicio */
	/* Mecanicos */
	mechanic_t *m;		/* Mecanicos del taller de reparación */
	/* Cantidades */
	unsigned int N; 	/* # total de maquinas operativas */
	unsigned int S; 	/* # total de maquinas de servicio */
	unsigned int M; 	/* # de mecanicos */
	unsigned int Tf;	/* Tiempo medio de fallo de una lavadora */
	unsigned int Tr;	/* Tiempo medio de raparación de una lavadora */
	int time;		/* Tiempo operativo continuo de la lavandería */
	/* Auxiliares */
	int s;			/* Última máquina de servicio lista + 1 */
	bool failure;		/* ¿Hay fallo de sistema? */
}



/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~~~~~~~~~~~~     FUNCIONES PRIVADAS     ~~~~~~~~~~~~~~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */



/* Traemos a servicio las maquinas recién reparadas */
static void get_from_mechanics (laundry_t l)
{
	unsigned int i = 0, last = 0;
	wm_t RMachine = NULL;	/* Lavadora reparada */
	
	INV
	
	last = l->s;
	for (i=0 ; i < l->M ; i++) {
		/* Le preguntamos al mecánico si ya tiene una lavadora lista */
		RMachine = mechanic_get_rm (l->m[i], l->time);
		if (RMachine != NULL) {
			/* Reparó una => la pasamos a servicio */
			l->serv_machines[last] = RMachine;
			last++;
		}
	}
	return;
}



/* Lleva al taller las lavadoras operativas que acaban de romperse
 * PRE:
 *	INV
 * POS:
 *	Toda máquina referenciada en la lista de operativas de la lavandería
 *	tiene tiempo de ruptura > tiempo actual. En código:
 *	(l->op_machines[i]).nbt > l->time  |
 *	ó sino l->op_machines[i] == NULL   | para todo i
 */
static void give_to_mechanics (laundry_t l)
{
	int i = 0;
	wm_t BMachine = NULL;	/* Lavadora rota */
	
	INV
	
	/* Quitamos las lavadoras que se rompieron este mes */
	for (i=0 ; i < l->N ; i++) {
		BMachine = (l->op_machines[i]).nbt == l->time ? \
				l->op_machines[i] : NULL;
		/* Si la i-ésima máquina está rota... */
		if ( BMachine != NULL ) {
			/* ...la sacamos de entre las operativas... */
			l->op_machines[i] = NULL;
			/* ...y la mandamos a arreglar */
			repair_machine (l, BMachine);
		}
	}
	
	return;
}



/* Mete en el taller la máquina lavadora pasada como 2º argumento,
 * entregándosela al mecánico con cola de reparación más corta
 * PRE:
 *	INV
 *	machine != NULL
 * POS:
 *	la lavadora fue puesta en la cola de reparación (o entregada
 *	directamente) del mecánico menos ocupado
 */
static void repair_machine (laundry_t l, wm_t machine)
{
	int i = 0, pos = -1, min = INT_MAX;
	mechanic_t m = NULL;
	
	INV
	assert (machine != NULL);
	
	/* Buscamos al mecánico con cola de reparación más corta... */
	for (i=0 ; i<M ; i++) {
		mech_q_len = mechanic_get_n_rmachines (l->m[i]);
		if (min > mech_q_len) {
			min = mech_q_len;
			pos = i;
		}
	}
	/* ...y le damos la lavadora para que la arregle */
	mechanic_repair_machine (l->m[pos], machine);
	
	return;
}



/* Reemplazamos los espacios vacíos entre las máquinas operativas
 * con las máquinas lavadoras de servicio 
 * PRE:
 *	INV
 * POS:
 *	Toda máquina referenciada en la lista de operativas de la lavandería
 *	tiene tiempo de ruptura > tiempo actual, ó sino la lavandería acaba
 *	de incurrir en un fallo de sistema
 *	
 * NOTE: ÉSTA ES LA FUNCIÓN QUE REVELA LOS FALLOS DE SISTEMA
 */
static void bring_to_operation (laundry_t l)
{
	int i = 0;
	
	INV
	
	for (i=0 ; i < l->N ; i++) {
		/* Si esta lavadora estaba rota y la llevaron al taller... */
		if (l->op_machines[i] == NULL) {
			if (l->s > 0) {
				/* ...la reemplazamos por una de servicio */
				l->s--;
				l->op_machines[i] = l->serv_machines[l->s];
			} else {
			/* Si ya no hay en servicio => FALLO DEL SISTEMA */
				l->failure = true;
				break;
			}
		}
	}
	return;
}



static void wash (void) { printf ("washing washing...\n"); return; }



/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~~~~~~~~~~~~     FUNCIONES PÚBLICAS     ~~~~~~~~~~~~~~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */



/* Creador de TAD. Deberá luego ser liberado con el destructor aquí provisto
 * Especificación de los parámetros:
 *	Nop:	# de máquinas operativas
 *	Nserv:	# de máquinas de servicio
 *	Nmech:	# de mecánicos en el taller de reparación
 *	Tf:	tiempo medio de fallo de una lavadora operativa
 *	Tr	tiempo medio de reparación de un mecánico
 */
laundry_t laundry_create (uint Nop, uint Nserv, uint Nmech, uint Tf, uint Tr)
{
	laundry_t l = NULL;
	
	l = (laundry_t) malloc (sizeof (struct _laundry));
	assert (l != NULL);
	
	l->N  = Nop;
	l->S  = Nserv;
	l->M  = Nmech;
	l->Tf = Tf;
	l->Tr = Tr;
	l->all_machines  = create_machines (Nop + Nserv);
	l->op_machines	 = (wm_t *) malloc (Nop   * sizeof(wm_t));
	l->serv_machines = (wm_t *) malloc (Nserv * sizeof(wm_t));
	l->s = Nserv;
	l->m = create_mechanics (Nmech);
	l->time = 0;
	l->failure = false;
	
	/* Al término de la creación aseguramos el invariante de la lavandería */
	INV
	
	return l;
}



/* Destructor del TAD
 * PRE:
 *	l != NULL
 * POS:
 *	todos los recursos de memoria de 'l' fueron liberados
 */
laundry_t laundry_destroy (laundry_t l)
{
	assert (l != NULL);
	
	l->all_machines = destroy_machines (l->all_machines);
	free (l->op_machines);   l->op_machines   = NULL;
	free (l->serv_machines); l->serv_machines = NULL;
	l->m = destroy_mechanics (l->m);
	
	free(l); l = NULL;

	return l;
}



/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas operativas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	INV
 */
void laundry_wash_clothes (laundry_t l)
{
	INV
	
	/* Traemos a servicio las maquinas recién reparadas */
	get_from_mechanics (l);
	/* Llevamos al taller las maquinas recién rotas */
	give_to_mechanics (l);
	/* Reemplazamos los vacios con las maquinas de servicio */
	bring_to_operation (l);
#ifdef _WASH
	/* Lavamos la ropa */
	wash();
#endif
	return;
}



/* Incrementa en uno el mes actual de la lavandería
 * PRE:
 *	INV
 * POS:
 *	el tiempo de operación de la lavandería fue incrementado en una unidad
 */
int laundry_increase_month (laundry_t laundry) { INV l->time++; return; }



/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * lavadoras en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t l) { assert (l!=NULL); return l->failure; }



/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * REQUIRES:
 *	l != NULL
 * RETURNS
 * 	ftime >=0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l)
{
	assert (l!=NULL);
	
	if (laundry_failure (l))
		return l->time;
	else
		return -1;
}


