#ifndef LAUNDRY_H
#define LAUNDRY_H


typedef struct _laundry *laundry_t;


/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas operativas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	l != NULL
 */
void wash_clothes (laundry_t *l);



/* Incrementa en uno el mes actual de la lavandería
 * PRE:
 *	l != NULL
 * POS:
 *	el tiempo de operación de la lavandería fue incrementado en una unidad
 */
int laundry_increase_month (laundry);





/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t *l);


/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * PRE:
 *	l != NULL
 * POS:
 * 	ftime >=0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l);


#endif
