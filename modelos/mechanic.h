#ifndef MECHANIC_H
#define MECHANIC_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "washingm.h"
#include "rg.h"		

typedef struct _mechanic * mechanic_t;


/* Funcion constructora del mecanico
 * REQUIRES:
 * 	tr	tiempo medio de reparacion (TR)
 * RETURNS:
 * 	NULL 			on error
 * 	mechanic != NULL	if success
 */
mechanic_t mechanic_create(double tr);

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

/* Funcion que devuelve la cantidad de maquinas que tiene el mecanico 
 * actualmente (tanto en la cola como la que esta reparando
 * REQUIRES:
 * 	m != NULL
 */
int mechanic_get_n_rmachines(mechanic_t m);

/* Funcion destructora 
 * REQUIRES:
 * 	m 	!= NULL
 */
mechanic_t mechanic_destroy(mechanic_t m);



#endif
