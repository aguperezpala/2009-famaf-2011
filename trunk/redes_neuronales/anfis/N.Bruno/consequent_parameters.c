#include <stdlib.h>
#include <float.h>
#include "consequent_parameters.h"

/* Aquí DEFINIMOS la variable que contiene los parámetros consecuentes
 * para que la red trabaje luego del aprendizaje
 *
 * El arreglo debe tener 'i' filas y 'j' columnas, donde:
 *	i == # de ramas de la red (ie: T^N para Jang; T para Bruno)
 *	j == # de entradas de la red + 1 (ie: N+1)
 *
 * Por los valores de 'N' y 'T' ver el archivo "trabajo_final.sh"
 *
 * Para que no se lea este archivo de manera que los consequent parameters
 * de la red no sean modificados definir:
 * double cp[]  =  {-DBL_MAX};
 */

double cp[]  =  {-DBL_MAX};
