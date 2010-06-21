#ifndef ACTIVIDAD_3_H
#define ACTIVIDAD_3_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "file_reader.h"
#include "actividad2.h"

/* Funcion que imprime los parametros de la normal.
 * REQUIRES:
 * 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
 *	size	(tamaño de datos a leer)
 * RETURNS:
 * 	< 0	on error
 * 	0	if success
 */
int act3_normal_params(const char *fname, int size);

/* Funcion que imprime los parametros de la log-normal.
* REQUIRES:
* 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
*	size	(tamaño de datos a leer)
* RETURNS:
* 	< 0	on error
* 	0	if success
*/
int act3_lnNormal_params(const char *fname, int size);

/* Funcion que imprime los parametros de la gamma.
* REQUIRES:
* 	fname	!= NULL (nombre del archivo donde se encuentran los datos)
*	size	(tamaño de datos a leer)
* RETURNS:
* 	< 0	on error
* 	0	if success
*/
int act3_gamma_params(const char *fname, int size);


#endif
