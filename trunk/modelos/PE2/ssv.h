/* Valores estadisticos muestrales (Sample Statistical Values)		      *
 *									      *
 * Modo de uso: 1) Invocar media_m para el paso n			      *
 *		2) Invocar var_m para el paso n 			      *
 *									      *
 * NOTE: Las llamadas a media_m y var_m deben realizarse SIEMPRE en ese orden *
 *	 incluso si no se desea calcular el valor de la media muestral 	      *
 *									      *
 *	 Antes de iniciar una nueva simulacion y recomenzar desde el paso 1   *
 *	 a calcular medias y varianzas debe invocarse a las funciones reset_  */

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


/* Reinicializa la media, como si volviesemos al paso 0 */
void reset_media_m (void);


/* Reinicializa la varianza, como si volviesemos al paso 0 */
void reset_var_m (void);


/* Método de Bootstrap para estimar el Error Cuadrático Medio (ECM)
 * del estimador "Media muestral" con respecto a la verdadera media µ,
 * dada una muestra de 'n' datos pasada como primer argumento
 *
 * Utiliza Montecarlo para devolver un valor que aproxima al ECM exacto
 *
 * PRE: sample != NULL
 */
double bootstrap_media (double *sample, unsigned int n);


#endif
