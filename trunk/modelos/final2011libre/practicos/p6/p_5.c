#include <stdio.h>
#include <sys/time.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "gen_continuas.h"

#define CANT_ITERACIONES	1000


/*! Este es un problema donde hay que estimar por proporcion = (cant exitos)/(cant total)
 * Donde cada simulacion (o variable observada) puede valer 1 con probabilidad p
 * o 0 con 1-p = q.
 * el promedio (X|(n)) es insesgado para p.
 * la varianza(X|(n)) = p*(1-p)/n. (por lo que la desv.estandar = sqrt(varianza)).
 * La varianza se calcula con el X|(n) en vez de p ==> Var = X|(n)*(1 - X|(n))/n
 */

/* Esta funcion nos devuelve exito (1) si el punto generado en el cuadrado
* (-1,1), (-1,-1), (1,1), (1,-1), esta dentro del circulo de radio 1.
* Osea si x*x + y*y <= 1 con (x,y) el punto generado.
* Devuelve 0 en caso de que no pertenezca
*/
int pertenecePuntoAlCirculo(void)
{
	double X = 0.0, Y = 0.0;
	
	X = rg_gen_uni_cont(-1,1);
	Y = rg_gen_uni_cont(-1,1);
	
	if(sqrt(X*X + Y*Y) <= 1)
		return 1;
	
	return 0;
}
int main5(int argc, char * argv[])
{
	double Xobservada = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double result = 0;
	double mitadLongIntervalo = 0;
	int n = 0;
	
		
	for(n = 1; n <= CANT_ITERACIONES ; n++){
		Xobservada = pertenecePuntoAlCirculo();
		result += Xobservada;
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
	}
	
	mitadLongIntervalo = 1.96 * sqrt(Sj1 / (double)n);

	/*! Notar que el valor de PI = 4*E[X] ya que hay 4 cuadrantes:
	* Porque la probabilidad de caer en alguno de los cuadrantes 
	* de cada punto generado es pi/4 = p --> pi = 4*p
	* */
	
	printf("Valor exacto de (PI) = %f\n", M_PI);
	printf("Valor estimado de PI (4*E[X]) = %f\n", 4*Xpromj1);
	printf("La varianza muestral = %f\n", Sj1);
	printf("El intervalo de confianza [%f ; %f]\n", Xpromj1-mitadLongIntervalo,
		Xpromj1+mitadLongIntervalo);

	return 0;
}