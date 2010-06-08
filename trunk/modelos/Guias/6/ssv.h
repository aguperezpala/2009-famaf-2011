/* Valores estadisticos muestrales (Sample Statistical Values)		      *
 *									      *
 * Modo de uso: 1) Invocar media_m para el paso n			      *
 *		2) Invocar var_m para el paso n 			      *
 *									      *
 * NOTE: las llamadas a media_m y var_m deben realizarse SIEMPRE en ese orden *
 *	 incluso si no se desea calcular el valor de la media muestral 	      */

#ifndef SAMPLE_STATISTICAL_VALUES_H
#define SAMPLE_STATISTICAL_VALUES_H


/* Media muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la media muestral = X(n)
 */
double media_m (double Xn, double n);


/* Varianza muestral para el n-esimo paso. Toma una nueva muestra Xn y
 * devuelve el "proximo" valor de la varianza muestral = S^2(n)
 *
 * NOTE: ASUME que media_m fue llamado anteriormente para calcular el valor X(n)
 *	 de la media muestral del n-esimos paso, para el cual se quiere calcular
 *	 ahora la varianza muestral
 */
double var_m (double Xn, double n);


#endif