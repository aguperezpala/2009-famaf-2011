#include "mechanic.h"

struct _mechanic {
	int rrt; /* Tiempo absoluto hasta la proxima maquina que sale 
		    del taller */
	GQueue rq; /* Cola de espera de reparacion */
};

/* Funcion constructora del mecanico
* RETURNS:
* 	NULL 			on error
* 	mechanic != NULL	if success
*/
mechanic_t mechanic_create(void);

/* Funcion que devuelve una washing machine en caso de que haya una reparada
* o NULL en caso contrario.
* REQUIRES:
* 	month	(mes actual)
* 	m 	!= NULL
* RETURNS:
* 	wm	si hay maquina reparada
* 	NULL	si no hay 
*/
wm_t mechanic_get_rm(mechanic_t m, int month);

/* Funcion que agrega una maquina a reparar
* REQUIRES:
* 	m 	!= NULL
* 	wm	!= NULL
*/
void mechanic_repair_machine(mechanic_t m, wm_t wm);


/* Funcion destructora 
* REQUIRES:
* 	m 	!= NULL
*/
mechanic_t mechanic_destroy(mechanic_t m);



