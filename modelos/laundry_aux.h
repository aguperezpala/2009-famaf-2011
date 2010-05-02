#ifndef LAUNDRY_AUX_H
#define LAUNDRY_AUX_H

#include <stdio.h>
#include <stdlib.h>

#include "mechanic.h"
#include "washingm.h"

/* Funcion que castea el parametro p
 * RETURNS:
 * 	< 0 	on error
 * 	>=0	cc
 */
int get_param(uint p, char **argv);

/* Funcion que devuelve un arreglo de tamaño N de wm_t
 * RETURNS:
 *	NULL		on error
 * NOTE: genera un NULL al final de la lista para indicar eso
 */
wm_t *create_machines(uint N);

/* Funcion que devuelve un arreglo de tamaño N de mechanics
* RETURNS:
*	NULL		on error
* NOTE: genera un NULL al final de la lista para indicar eso
*/
mechanics_t *create_mechanics(uint N);

/* Destructor */
wm_t *destroy_machines(wm_t *wm);

/* Destructor */
mechanics_t *destroy_mechanics(mechanics_t *m);


/* Funcion que reinicializa toda la lavanderia
 * REQUIRES:
 * 	opm 	!= NULL
 * 	bm	!= NULL
 * 	sm	!= NULL
 * 	m	!= NULL
 */
void reset_laundry(wm_t *opm, wm_t *bm, wm_t *sm, mechanics_t *m);


/* Obtenemos las máquinas que se acaban de romper y las sacamos de entre las 
 * operativas 
 * REQUIRES:
 * 	opm 	!= NULL
 * 	bm	!= NULL
 *	bo	!= NULL
 */
uint get_broken(wm_t *opm, unsigned long time, wm_t *bm, wm_t *bo);

/* Funcion que obtiene la sumatoria de las maquinas que se estan arreglando en
 * los diferentes mecanicos (m)
 * REQUIRES:
 * 	m 	!= NULL
 */
uint get_repairing(mechanics_t *m);

/* Traemos a servicio las maquinas recién reparadas
 * REQUIRES:
 * 	m 	!= NULL
 * 	sm 	!= NULL
 */
void get_from_mechanics (mechanics_t *m, wm_t *sm);

/* Reemplazamos los vacios con las maquinas de servicio 
 * REQUIRES:
 * 	sm 	!= NULL
 * 	om	!= NULL
 */
void bring_to_operation (wm_t *sm, wm_t *om)

/* Llevamos al taller las maquinas recién rotas 
* REQUIRES:
* 	bm 	!= NULL
* 	m	!= NULL
*/
void give_to_mechanics (wm_t *bm, mechanic_t *m);



#endif
