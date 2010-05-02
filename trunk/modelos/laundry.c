#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "mechanic.h"
#include "washingm.h"



#define INV assert(l != NULL); \
	    assert(l->all_machines != NULL); \
	    assert(l->op_machines != NULL); \
	    assert(l->serv_machines != NULL); \
	    assert(l->broken_machines != NULL); \
	    assert(l->m != NULL); \
	    assert(l->broken_order != NULL);



struct _laundry {
	/* Máquinas lavadoras */
	wm_t *all_machines;	/* Todas las máquinas */
	wm_t *op_machines;	/* Referencia a las maquinas operativas */
	wm_t *serv_machines;	/* Referencia a las maquinas de servicio */
	wm_t *broken_machines;	/* Referencia a las maquinas actualmente rotas */
	/* Mecanicos */
	mechanic_t *m;		/* Mecanicos */
	/* Cantidades */
	unsigned int N; 	/* # total de maquinas operativas */
	unsigned int S; 	/* # total de maquinas de servicio */
	unsigned int M; 	/* # de mecanicos */
	unsigned int Tf;	/* Tiempo medio de fallo de una lavadora */
	unsigned int Tr;	/* Tiempo medio de raparación de una lavadora */
	int time;		/* Tiempo de operación de la lavandería */
	/* Punteros a los arreglos */
	int *broken_order;	/* Indices de orden */
	int s;			/* Primera máquinas de servicio libre */
}




/* Traemos a servicio las maquinas recién reparadas */
static void get_from_mechanics (laundry_t *l)
{
	unsigned int i = 0;
	wm_t RMachine = NULL;
	
	INV
	
	for (i=0 ; i < l->M ; i++) {
		/* Le preguntamos al mecánico si ya tiene una lavadora lista */
		RMachine = mechanic_get_rm (l->m[i], l->time);
		if (RMachine != NULL) {
			/* Reparó una => la pasamos a servicio */
			l->serv_machines[l->s] = RMachine;
			l->s++;
		}
	}
	return;
}




/* Lleva al taller las lavadoras que acaban de romperse */
static void give_to_mechanics (laundry_t *l)
{
	int i = 0, j = 0;
	wm_t BMachine = NULL;	/* Lavadora rota */
	
	INV
	
	/* Reinicializamos la lista de lavadoras rotas */
	memset ( l->broken_order, -1, N * sizeof(int));
	
	/* Qiutamos las lavadoras que se rompieron este mes */
	for (i=0 ; i < l->N ; i++) {
		BMachine = (l->op_machines[i]).nbt == l->time ? \
				l->op_machines[i] : NULL;
		/* Si la máquina está rota... */
		if ( BMachine != NULL ) {
			/* ...lo registramos... */
			l->broken_machines[j] = BMachine;
			l->broken_order[j] = i;
			j++;
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
 */
static void repair_machine (laundry_t *l, wm_t machine)
{
	int i = 0, pos = -1, min = INT_MAX;
	mechanic_t m = NULL;
	
	INV
	
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


/* Reemplazamos los vacios con las maquinas de servicio */
static void bring_to_operation (laundry_t *l)
{
	int i = 0;
	
	INV
	
	for (i=0 ; i < l->N ; i++) {
		/* Si esta lavadora estaba rota y la llevaron al taller... */
		if (l->op_machines[i] == NULL) {
			/* ...la reemplazamos por una de servicio */
			if (l->s > 0) {
				l->op_machines[i] = l->serv_machines[(l->s)-1];
				l->s--;
			} else
			/** Si ya no hay en servicio => FALLO DEL SISTEMA */
				break;
		}
	}
	
	return;
}



/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	INV
 */
void wash_clothes (laundry_t *l)
{
	INV
	
	/* Traemos a servicio las maquinas recién reparadas */
	get_from_mechanics (l);
	/* Llevamos al taller las maquinas recién rotas */
	give_to_mechanics (l);
	/* Reemplazamos los vacios con las maquinas de servicio */
	bring_to_operation (l);
	/* Lavamos la ropa */
	wash();
	
	return;
}


static void wash (void)
{
#ifdef _WASH
	printf ("Washing, washing...\n");
#endif
	return;
}


/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * lavadoras en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t *l)
{
	int i = 0;
	
	INV
	
	for (i=0 ; i<l->N ; i++) {
		if (l->op_machines[i] == NULL)
			return true;
	}
	
	return false;
}



/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * REQUIRES:
 *	l != NULL
 * RETURNS
 * 	ftime > 0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l)
{
	INV
	
	if (laundry_failure (l))
		return l->time;
	else
		return -1;
}


