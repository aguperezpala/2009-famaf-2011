#ifndef MEMBERSHIP_FUNCTIONS_H
#define MEMBERSHIP_FUNCTIONS_H


/* NOTE: Aquí definimos las modificaciones arbitrarias que se realizarán
 *	 sobre los parámetros iniciales de las funciones membresía
 */


/* Pendiente de las funciones membresía (parámetro 'b' de la MF tipo "bell") */
#define  SLOPE		2.0


/* Modificación del rango de datos de entrada                              *
 * RANGEXP = 1.0: se cubrirá exactamente el rango de los datos de entrada  *
 * RANGEXP < 1.0: se cubrirá menos rango                                   *
 * RANGEXP > 1.0: se cubrirá más rango                                     */
#define  RANGEXP	1.5


/* Modificación del ancho de cada función membresía  *
 * AWIDTH = 1.0: se mantiene el ancho "óptimo"       */
 #define  AWIDTH	1.1


/* Desplazamiento del centro de cada función membresía  *
 * en exactamente 'DECENTRE' unidades                   *
 * DECENTRE = 0.0: se mantiene el centrado "óptimo"     */
#define  DECENTRE	0.0


/* Modificación del solapamiento entre funciones membresía         *
 * pertenecientes a una misma rama                                 *
 * JUNCTION_1 = 0.0: se mantiene el solapamiento "óptimo"          *
 * JUNCTION_1 > 0.0: las MF's de cada rama se superponen entre sí  *
 * JUNCTION_1 < 0.0: las MF's de cada rama se separan entre sí     */
#define  JUNCTION_1	-2.0


/* Modificación del solapamiento entre las funciones membresía             *
 * de las distintas ramas                                                  *
 * JUNCTION_2 = 0.0: se mantiene el solapamiento "óptimo"                  *
 * JUNCTION_2 > 0.0: las MF's de todas las rama tienden a un centro común  *
 * JUNCTION_2 < 0.0: el dominio cubierto por las MF's de una rama          *
 *		     es disjunto al de las demás ramas                     */
#define  JUNCTION_2	1.2


#endif
