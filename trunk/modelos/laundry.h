#ifndef LAUNDRY_H
#define LAUNDRY_H


typedef struct _laundry *laundry_t;



/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t *l);


/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * REQUIRES:
 *	l != NULL
 * RETURNS
 * 	ftime > 0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l);


/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	INV
 */
void wash_clothes (laundry_t *l);



#endif
