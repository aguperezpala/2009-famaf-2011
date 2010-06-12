#include <stdio.h>
#include <stdbool.h>
#include "rdg.h"
#include "ssv.h"

/** Variables globales */

#define Q 4	/* maximo # permitido de clientes simultaneos en el sistema */

double q[Q];	/* cola de espera del servidor */

unsigned int first = 0,	/* Posicion del primer cliente de la cola */
	     last = 0;	/* Posicion del ultimo cliente de la cola */

/* NOTE: q[first] == "cliente actualmente atendido por el servidor"
 *	 q[first+1 ... last] == "clientes en cola de espera"
 */

double	T = 8.0,	/* Periodo de tiempo de atencion del servidor */
	Ta = 4.0,	/* Tiempo medio de llegada de clientes */
	Ts = 4.2;	/* Tiempo medio de servicio del servidor */



/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~ FUNCIONES AUXILIARES PARA MANEJO DE COLA DE ESPERA ~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */



/* Indica si la cola de espera esta actualmente vacia */
static bool q_is_empty (void) { return first == last; }


/* Indica si la cola de espera esta actualmente llena */
static bool q_is_full (void) { return (first == (last + 1) % Q); }


/* Mete un nuevo cliente (con tiempo de arribo absoluto == 'ta')
 * al final de la cola global 'q'
 * PRE: ! q_is_full ()
 */
static void q_enqueue (double ta)
{
	assert(!q_is_full());
	
	last = (last + 1) % Q;
	q[last] = ta;
}


/* Devuelve el tiempo de arribo absoluto al sistema
 * del cliente que actualmente esta siendo atendido en el servidor
 *
 * PRE: ! q_is_empty ()
 */
static double q_first (void) { assert(!q_is_empty()); return q[first]; }


/* Quita el primer elemento de la cola global 'q',
 * y devuelve su tiempo absoluto de arribo al sistema 'ta'
 * PRE: ! q_is_empty ()
 */
static double q_dequeue (void)
{
	assert(!q_is_empty());
	
	first = (first + 1) % Q;
	
	if (first == 0)
		return q[Q-1];
	else
		return q[first-1];
}


/** ------------------------------------------------------------------------- *
 ** ~~~~~~~ FUNCIONES AUXILIARES PARA ATENCION/RECEPCION DE CLIENTES ~~~~~~~~ *
 ** ------------------------------------------------------------------------- */


/* Atiende al cliente que actualmente esta usando al servidor 
 *
 * PRE: *tsal == tiempo (absoluto) de salida del servidor del cliente actual
 *
 * busy = serve_customer (&tsal, &wtime)
 *
 * POS: *wtime == tiempo total que estuvo en el sistema
 *		  el cliente que acaba de ser atendido
 *
 *	 busy => *tsal == tiempo (absoluto) de salida del servidor
 *			  del proximo cliente
 *
 *	!busy => el servidor se quedo sin clientes
 *
 */
static bool serve_customer (int *tsal, double *wtime)
{
	bool busy = true;
	
	/* Sacamos al cliente del servidor y registramos el tiempo total
	 * que estuvo dentro del sistema */
	*wtime = *tsal - q_dequeue ();
	
	if (! q_is_empty() )
		/* Generamos un tiempo (absoluto) de salida del servidor para
		 * el proximo cliente que sera atendido */
		*tsal = q_first () + gen_exp (Ts);
	else
		busy = false;
	
	return busy;
}



/** ------------------------------------------------------------------------- *
 ** ### ### ### ### ### RUTINA PRINCIPAL DEL SERVIDOR ### ### ### ### ### ### *
 ** ------------------------------------------------------------------------- */



int main (void)
{
	int i = 0;
	
	/* Variables relacionadas con la simulacion */
	double	ta = 0.0;	/* Tiempo (absoluto) de arribo del proximo *
				 * cliente al sistema			   */
	double	tsal = 0.0;	/* Tiempo (absoluto) de salida del cliente *
				 * que actualmente esta siendo atendido    */
	double wtime = 0.0;	/* Tiempo que estuvo en el sist. un cliente */
	bool busy = false;	/* Servidor ocupado */
	
	/* Variables relacionadas con los resultados de las simulaciones */
	double	n = 0.0,	/* # de evento actual */
		X = 0.0,	/* media muestral en el paso 'n' */
		S = 0.0;	/* desv. est. muestral en 'n' */
	
	
	/* Simulamos el arribo de los clientes mientras TIEMPO_ABSOLUTO < 8 */
	ta = 0.0;
	tsal = 0.0;
	n = 0.0;
	busy = false;
	while (ta < T) {
		
		/* Tiempo absoluto de arribo del proximo cliente */
		ta += gen_exp (Ta);
		
		if (ta >= T)
			/* Se acabo el horario de recepcion de clientes */
			continue;
		
		while (ta > tsal && busy) {
			
			/* Ocurre antes la atencion del actual cliente en el
			 * servidor que el arribo del proximo cliente */
			busy = serve_customer (&tsal, &wtime);
			
			/* Registramos el hecho estadistico */
			n += 1.0;
			X = media_m (wtime, n);
			S = var_m (wtime, n);
		}
		
		/* Metemos al cliente en el servidor */
		q_enqueue (ta);
		
		if (!busy) {
			/* El servidor estaba vacio => atendemos directamente */
			busy = serve_customer (&tsal, &wtime);
		
			/* Registramos el hecho estadistico */
			n += 1.0;
			X = media_m (wtime, n);
			S = var_m (wtime, n);
		}
		
	}
	
	
	
	
	return 0;
}
