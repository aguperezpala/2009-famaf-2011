#ifndef LAUNDRY_H
#define LAUNDRY_H


typedef struct _laundry *laundry_t;


/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * REQUIRES:
 *	l != NULL
 * RETURNS
 * 	ftime > 0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
int laundry_get_failure_time (laundry_t l);


#endif
