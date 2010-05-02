/* TAD laundry, en español "lavandería"
 *
 * Contiene las máquinas lavadoras operativas, las de servicio
 * y el conjunto de mecánicos en el taller de reparación.
 * Lleva además la cuenta de los meses transcurridos desde su creación
 */


#ifndef LAUNDRY_H
#define LAUNDRY_H



typedef struct _laundry *laundry_t;



/* Creador de TAD. Deberá luego ser liberado con el destructor aquí provisto
 * Especificación de los parámetros:
 *	Nop:	# de máquinas operativas
 *	Nserv:	# de máquinas de servicio
 *	Nmech:	# de mecánicos en el taller de reparación
 *	Tf:	tiempo medio de fallo de una lavadora operativa
 *	Tr	tiempo medio de reparación de un mecánico
 */
laundry_t laundry_create (uint Nop, uint Nserv, uint Nmech, uint Tf, uint Tr);


/* Destructor del TAD
 * PRE:
 *	l != NULL
 * POS:
 *	todos los recursos de memoria de 'l' fueron liberados
 */
void laundry_destroy (laundry_t l);


/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas operativas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	l != NULL
 */
void laundry_wash_clothes (laundry_t l);


/* Incrementa en uno el mes actual de la lavandería
 * PRE:
 *	l != NULL
 * POS:
 *	el tiempo de operación de la lavandería fue incrementado en una unidad
 */
int laundry_increase_month (laundry_t laundry);


/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t l);


/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * PRE:
 *	l != NULL
 * POS:
 * 	ftime >=0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l);


#endif
