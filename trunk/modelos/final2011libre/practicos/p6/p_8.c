#include <stdio.h>
#include <sys/time.h>
#include <stdbool.h>
#include <math.h>


#include "analisis_estadistico.h"
#include "procesos_poisson.h"
#include "gen_continuas.h"

#define CANT_ITERACIONES	1000
#define TIEMPO_FUNC_SERVER	8
#define FREC_TIEMPO_ARRIBOS	4
#define FREC_TIEMPO_SERVICIO	4.2
#define CANT_CLIENT_SIMULTANEO	3	/* cantidad de clientes que puede atender el server */

/* definimos una estructura para simular un cliente, el tiempo de llegada,
 * el tiempo que permanece en el sistema y si realmente pudo entrar al sistema
 */
typedef struct {
	double tiempoEntrada;
	double tiempoSistema;	/* tiempo que permanecio en el sistema */\
	bool entroAlSistema;
} cliente_t;


/* funcion que determina la cantidad de clientes que hay en el sistema hasta
 * un tiempo determinado (tiempo). Toma la cantidad de clientes y verifica hasta n
 * clientes */
int contarClientesEnSistema(cliente_t *clientesAnteriores, int n, double tiempo)
{
	int i = 0;
	double aux = 0.0;
	int counter = 0;
	
	for(i = 1; i < n; i++) {
		if(!clientesAnteriores[i].entroAlSistema)
			continue;
		
		aux = clientesAnteriores[i].tiempoEntrada + 
			clientesAnteriores[i].tiempoSistema;
		if(aux >= tiempo)
			counter++;
	}
	
	return counter;
}


/* funcion que genera una simulacion del proceso, devuelve el tiempo promedio
 * que un cliente pasa en el sistema */
double simular_server(void)
{
	cliente_t clientes[130]; /* por si las moscas aunque es imposible */
	int i = 1;
	double tiemposArribos[130];
	int cantTiempoArribos = 0;
	bool entraAlSist = false;
	double result = 0;
	double aux = 0;
	int counter = 0;
	double Muestras[130];	/* vector utilizado para guardar las Xi */
	double MUempirico = 0;
	double ECMempirico = 0;
	
	/* calculamos los arribos */
	cantTiempoArribos = process_poisson_homogeneo(TIEMPO_FUNC_SERVER, 
						     FREC_TIEMPO_ARRIBOS, 
						     tiemposArribos);
	
	/* ahora generamos para cada uno de los tiempos de arribo los clientes
	 * y les seteamos el tiempo de servicio */
	clientes[i].tiempoEntrada = tiemposArribos[i];
	clientes[i].tiempoSistema = rg_gen_exp(FREC_TIEMPO_SERVICIO);
	entraAlSist = (contarClientesEnSistema(clientes,
					       i, clientes[i].tiempoEntrada)
					       <= CANT_CLIENT_SIMULTANEO);
					       clientes[i].entroAlSistema = entraAlSist;
	for(i = 2; i <= cantTiempoArribos; i++) {
		/* veamos que en realidad va a empezar un cliente a ser atendido
		 * cuando el otro sea liberado, ya que la atencion a los clientes
		 * es secuencial, no paralela */
		clientes[i].tiempoEntrada = tiemposArribos[i];
		clientes[i].tiempoSistema = rg_gen_exp(FREC_TIEMPO_SERVICIO);
		/* verificamos si el tiempo de servicio del cliente anterior se 
		* superpone con nuestra llegada y de ser asi deberiamos correr
		* el tiempo en el sistema */
		aux = clientes[i-1].tiempoEntrada + clientes[i-1].tiempoSistema;
		if(aux > clientes[i].tiempoEntrada){
			/* le debemos agregar el Delta time */
			clientes[i].tiempoSistema += (aux - clientes[i].tiempoEntrada);
		}
			
		entraAlSist = (contarClientesEnSistema(clientes,
						       i, clientes[i].tiempoEntrada)
						       <= CANT_CLIENT_SIMULTANEO);
						       clientes[i].entroAlSistema = entraAlSist;
		
	}
	
	/* calculamos el promedio de tiempo en el sistema de los clientes */
	for(i = 1; i <= cantTiempoArribos; i++) {
		if(!clientes[i].entroAlSistema)
			continue;
		
		
		Muestras[counter] = clientes[i].tiempoSistema;
		counter++;
		result += clientes[i].tiempoSistema;
	}
	
	/*! notar que aca deberiamos calcular el error cuadratico medio por medio
	 * de boostrap, recordando ECMempirico[X| , MUempirica] = 
	 * Sumatoria .... Sumatoria pow((g(Xi1,..,Xin) - MUempirica),2)/pow(n,n)
	 * from i1=1 to n.... from in=1 to n
	 * En este caso g(Xi1,..,Xin) es la media muestral X|.
	 * Deberiamos calcular entonces MUempirica = Sumatoria Xi/n from i=1 to n
	 * de los datos muestrales.
	 */
	for(i = 1; i < counter; i++) {
		MUempirico += Muestras[i];
	}
	MUempirico /= (double)counter;
	ECMempirico = bootstrap_media(Muestras, counter);
	
	printf("MUEmpirico: %G\n", MUempirico);
	printf("ECMempirico: %G\n", ECMempirico);
	
	return (result / (double) counter);
}


int main(int argc, char * argv[])
{
	double Xobservada = 0, Xpromj = 0, Xpromj1 = 0;
	double Sj = 0, Sj1 = 0;
	double desvEst = 0;
	double result = 0;
	double mitadLongIntervalo = 0;
	int n = 0;
	
		
	/*for(n = 1; n <= 2 ; n++){
		Xobservada = simular_server();
		result += Xobservada;
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		
		Xpromj = Xpromj1;
		Sj = Sj1;
	}
	*/
	
	/*printf("Media de tiempo de espera: %f\n", result/(double)n);
	printf("Valor estimado de tiempo de espera: %f\n", Xpromj1);
	printf("La varianza muestral = %f\n", Sj1);
	*/
	
	
	double sample[500];
	
	for(n = 0; n < 500; n++)
		sample[n] = 4;
	
	for(n = 0; n < 500; n++){
		Xobservada = sample[n];
		Xpromj1 = estimar_media_muestral(n, Xpromj, Xobservada);
		Sj1 = estimar_varianza_muestral(n, Sj, Xpromj1, Xpromj);
		/* Actualizamos los datos antiguos */
		Xpromj = Xpromj1;
		Sj = Sj1;
	}
	double mediaInef = estimar_media_muest_inef(sample, 500);
	printf("Media ineficiente : %G\t Varianza ineficiente : %G\n",
	       mediaInef, 
	       estimar_var_muest_inef(sample,500,mediaInef));
	printf("Xpromj1: %G\t Sj1: %G\n", Xpromj1, Sj1);
	printf("\n\n\nCALCULANDO ECM[Media] BOOSTRAP: %G\n", 
	       bootstrap_media(sample,500));
	       
	printf("\n\n\nCALCULANDO ECM[Varianza] BOOSTRAP: %G\n", 
		bootstrap_varianza(sample,500));

	       

	return 0;
}