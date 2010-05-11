#include <assert.h>
#include <stdbool.h>
#include "mechanic.h"


struct _mechanic {
	double rrt; 	/* Tiempo absoluto hasta la proxima maquina que sale 
			 * del taller */
	GQueue *rq; 	/* Cola de espera de reparacion */
	double TR;	/* tiempo medio de reparacion (1/lambda) */
};

/* Funcion constructora del mecanico
* REQUIRES:
* 	tr	tiempo medio de reparacion (TR)
* RETURNS:
* 	NULL 			on error
* 	mechanic != NULL	if success
*/
mechanic_t mechanic_create(double tr)
{
	mechanic_t result = NULL;
	
	result = (mechanic_t) malloc(sizeof(*result));
	if (!result)
		return NULL;
	
	/* creamos ahora la cola */
	result->rq = g_queue_new();
	if(!result->rq) {
		perror("Error al crear la cola\n");
		free(result);
		return NULL;
	}
	assert(g_queue_is_empty(result->rq));
	result->rrt = -1;
	result->TR = tr;
	
	return result;
}

/* Funcion que reinicializa el mecanico, limpiando y sacando todas las maquinas
* que actualmente tiene en "su poder"
* REQUIRES:
* 	m != NULL
*/
void mechanic_reinitialize(mechanic_t m)
{
	if (!m)
		assert(false);
	
	g_queue_clear(m->rq);
	assert(g_queue_is_empty(m->rq));
	m->rrt = -1;
}


/* Función que disminuye en "elapsed" unidades
 * el tiempo de reparación actual del mecánico
 * REQUIRES:
 *	m != NULL
 *	elpased <= m->rrt
 */
void mechanic_elapse_time (mechanic_t m, double elapsed)
{
	assert (m != NULL);
	assert (elapsed <= m->rrt);
	m->rrt -= elapsed;
	return;
}


/* Funcion que devuelve una washing machine en caso de que haya una
 * en reparación, o NULL en caso contrario.
 * REQUIRES:
 *	m 	!= NULL
 * RETURNS:
 * 	wm	si hay maquina reparada
 * 	NULL	si no hay 
 */
wm_t mechanic_get_rm (mechanic_t m)
{
	wm_t wm = NULL;
	
	if (!m) {
		printf("Error, estamos recibiendo un mecanico NULL\n");
		return NULL;
	}
	
	/* verificamos si tenemos o no maquinas que devolver */
	if (g_queue_is_empty(m->rq))
		return NULL;
	
	/* Si estabamos arreglando ==> la extraemos y la devolvemos */
	wm = (wm_t) g_queue_pop_head(m->rq);
	
	if (!wm) {
		/* que paso aca? como puede ser... */
		printf("Algo esta andando mal en el mecanico\n");
		assert(false);
	}

	/* Nos ponemos a reparar la siguiente (si hay una siguiente) */
	if (g_queue_is_empty(m->rq))
		/* no hay nada que reparar */
		m->rrt = -1;
	else {
		/* si hay maquinas => determinamos en cuanto
		* tiempo va a ser reparada, recordemos que 
		* la media de la exponencial es 1/lambda. 
		* y vamos a tomar el techo del valor.. */
		
		m->rrt = rg_gen_exp(1.0/m->TR);
	}
	
	/* si no teníamos ninguna máquina devolvemos NULL, si no devolvemos la
	 * que estaba primera */
	
	return wm;
}

/* Funcion que devuelve el tiempo en el que va a estar lista la proxima wm
* o -1 si no hay.
* REQUIRES:
* 	m != NULL
* RETURNS:
* 	>= 0 si no hay error (tiempo absoluto en el que va a estar arreglada
* 			      la proxima maquina)
*	< 0 si no hay mquinas
*/
double mechanic_get_rrt(mechanic_t m)
{
	assert(m != NULL);
	return m->rrt;
}

/* Funcion que agrega una maquina a reparar
* REQUIRES:
* 	m 	!= NULL
* 	wm	!= NULL
*/
void mechanic_repair_machine(mechanic_t m, wm_t wm)
{
	if (!m || !wm) {
		printf("Intentando agregar una maquina en un mecanico NULL "
			" o intentando meter una maquina NULL\n");
		return;
	}
	
	/* Si aún no estamos arreglando ninguna... (ie: si la cola es vacía) */
	
	if (g_queue_is_empty(m->rq))
		/* ...la arreglamos ya, y veamos cuanto va a tardar.. */
		m->rrt = rg_gen_exp (1.0/m->TR);
	
	/* si o si la encolamos */
	g_queue_push_tail(m->rq, wm);
}

/* Funcion que devuelve la cantidad de maquinas que tiene el mecanico 
* actualmente (tanto en la cola como la que esta reparando
* REQUIRES:
* 	m != NULL
*/
int mechanic_get_n_rmachines(mechanic_t m)
{
	if (!m){
		assert(false);
		return -1;
	}
	return g_queue_get_length(m->rq);
}

/* Funcion destructora 
* REQUIRES:
* 	m 	!= NULL
*/
mechanic_t mechanic_destroy(mechanic_t m)
{
	if (m) {
		g_queue_free(m->rq);
		free(m);
	}
	
	return NULL;
}


