#ifndef MECHANIC_H
#define MECHANIC_H

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "washingm.h"

typedef struct _mechanic * mechanic_t;


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



#endif
