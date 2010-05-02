#include "mechanic.h"

struct _mechanic {
	int rrt; 	/* Tiempo absoluto hasta la proxima maquina que sale 
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
		return;
	
	g_queue_clear(m->rq);
	m->rrt = -1;
}


/* Funcion que devuelve una washing machine en caso de que haya una reparada
* o NULL en caso contrario.
* REQUIRES:
* 	month	(mes actual)
* 	m 	!= NULL
* RETURNS:
* 	wm	si hay maquina reparada
* 	NULL	si no hay 
*/
wm_t mechanic_get_rm(mechanic_t m, int month)
{
	wm_t wm = NULL;
	
	
	if (!m) {
		printf("Error, estamos recibiendo un mecanico NULL\n");
		return NULL;
	}
	
	/* verificamos si tenemos o no maquinas que devolver */
	if (g_queue_is_empty(m->rq))
		return NULL;
	
	/* vamos a verificar si ya esta arreglada la maquina */
	if (m->rrt == month) {
		/* SI esta arreglada ==> la extraemos y la devolvemos */
		wm = (wm_t) g_queue_pop_head(m->rq);
		
		if (!wm) 
			/* que paso aca? como puede ser... */
			printf("Algo esta andando mal en el mecanico\n");
		else {
			/* debemos ver ahora si tenemos que ponernos a reparar
			 * la otra ahora, o si no tenemos ninguna no hacemos
			 * nada */
			if (g_queue_is_empty(m->rq))
				/* no hay nada que reparar */
				m->rrt = -1;
			else {
				/* si hay maquinas => determinamos en cuanto
				 * tiempo va a ser reparada, recordemos que 
				 * la media de la exponencial es 1/lambda. 
				 * y vamos a tomar el techo del valor.. */
				m->rrt = ceil(rg_gen_exp((double)1.0/m->TR)) 
							+ month;
			}
		}
	}
	
	/* si no tenemos ninguna maquina devolvemos NULL, si no, devolvemos la
	 * primera en la cola */
	
	return wm;
}

/* Funcion que agrega una maquina a reparar
* REQUIRES:
* 	m 	!= NULL
* 	wm	!= NULL
*/
void mechanic_repair_machine(mechanic_t m, wm_t wm, int month)
{
	if (!m || !wm) {
		printf("Intentando agregar una maquina en un mecanico NULL "
			" o intentando meter una maquina NULL\n");
		return;
	}
	
	/* veamos si tenemos arreglando una maquina.. osea si la cola es
	 * distinta de vacio */
	if (g_queue_is_empty(m->rq))
		/* la arreglamos ya ... y veamos cuanto va a tardar.. */
		m->rrt = ceil(rg_gen_exp((double) 1.0/m->TR)) + month;
	else
		/* hay maquinas arreglandose, simplemente la encolamos */
		g_queue_push_tail(m->rq, wm);
}

/* Funcion que devuelve la cantidad de maquinas que tiene el mecanico 
* actualmente (tanto en la cola como la que esta reparando
* REQUIRES:
* 	m != NULL
*/
int mechanic_get_n_rmachines(mechanic_t m)
{
	if (!m)
		return -1;
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


