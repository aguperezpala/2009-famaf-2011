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
laundry_t laundry_create (unsigned int Nop, unsigned int Nserv, 
			   unsigned int Nmech, double Tf, double Tr);


/* Destructor del TAD
 * PRE:
 *	l != NULL
 *	l fue creada con laundry_create()
 * POS:
 *	todos los recursos de memoria de 'l' fueron liberados
 */
laundry_t laundry_destroy (laundry_t l);


/* Simulación de una operación de la lavandería
 * desde el tiempo inicial = 0 hasta el primer fallo del sistema
 *
 * PRE:
 *	l != NULL
 *	l fue creada con laundry_create()
 * POS:
 *	Devuelve el tiempo transcurrido hasta el 1º fallo del sistema
 */
double laundry_simulation (laundry_t l);


/* Funcion que reseta toda la estructura de laundry, dejandola en 0km.
 * REQUIRES:
 * 	l != NULL
 *	l fue creada con laundry_create()
 */
void laundry_reset(laundry_t l);


#endif
