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

double cp[]  =  { -0.2167,   0.7233,  -0.0365,   0.5433,   0.0276,
		   0.2141,   0.5704,  -0.4826,   1.2452,  -0.3778,
		  -0.0683,   0.0022,   0.6495,    2.732,  -2.2916,
		  -0.2616,   0.9190,  -2.9931,   1.9467,   1.6555,
		  -0.3293,  -0.8943,   1.4290,   -1.655,   2.3735,
		    2.582,  -2.3109,   3.7925,  -5.8068,   4.0478,
		   0.8797,  -0.9407,   2.2487,   0.7759,  -2.0714,
		  -0.8417,  -1.5394,  -1.5329,   2.2834,    2.414,
		  -0.6422,  -0.4384,   0.9792,  -0.3993,   1.5593,
		   1.5534,  -0.0542,  -4.7256,   0.7244,    2.735,
		  -0.6864,  -2.2435,   0.1585,   0.5304,   3.5411,
		   -0.319,   -1.316,   0.9689,   1.4887,   0.7079,
		    -0.32,  -0.4654,   0.4880,  -0.0559,   0.9622,
		    4.022,  -3.8886,   1.0547,  -0.7427,  -0.4464,
		   0.3338,  -0.3306,  -0.5961,   1.1220,   0.3529,
		  -0.5572,    0.919,  -0.8745,   2.1899,  -0.9497
		};
