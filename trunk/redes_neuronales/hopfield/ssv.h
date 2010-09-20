/* Valores estadisticos muestrales (Sample Statistical Values)		      *
 *									      *
 * Modo de uso: 1) Invocar media_m para el paso n			      *
 *		2) Invocar var_m para el paso n 			      *
 *									      *
 * NOTE: Las llamadas a media_m y var_m deben realizarse SIEMPRE en ese orden  *
 *	 incluso si no se desea calcular el valor de la media muestral 	      *
 *									      *
 *	 Antes de iniciar una nueva simulacion y recomenzar desde el paso 1   *
 *	 a calcular medias y varianzas debe invocarse a las funciones reset_  */

#ifndef SAMPLE_STATISTICAL_VALUES_H
#define SAMPLE_STATISTICAL_VALUES_H


/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * calcula el "proximo" valor de la media muestral = X(n)
 */
void
media_m (double Xn, double n);


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * calcula el "proximo" valor de la varianza muestral = S^2(n)
 *
 * NOTE: ASUME que media_m fue llamado anteriormente para calcular el valor X(n)
 *	 de la media muestral del n-esimos paso, para el cual se quiere calcular
 *	 ahora la varianza muestral
 */
void
var_m (double Xn, double n);


/* Devuelve la última media muestral calculada */
double
get_media_m (void);


/* Devuelve la última varianza muestral calculada */
double
get_var_m (void);


/* Reinicializa la media, como si volviesemos al paso 0 */
void
reset_media_m (void);


/* Reinicializa la varianza, como si volviesemos al paso 0 */
void
reset_var_m (void);

#endif
