#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include "rdg.h"
#include "ssv.h"

/** Variables globales */

/* maximo # tolerado de arribos ("simulaciones") */
#define  N  (1<<12)

/* maximo # permitido de clientes simultaneos en el sistema */
#define  Q  4

double q[Q];	/* cola de espera del servidor */

unsigned int first = 0,	/* Posicion del primer cliente de la cola */
	     last = 0;	/* Posicion del ultimo cliente de la cola */

/* NOTE: q[first] == "cliente actualmente atendido por el servidor"
 *	 q[first+1 ... last] == "clientes en cola de espera"
 */

double	T = 8.0,	/* Periodo de tiempo de atencion del servidor */
	Ta = 4.0,	/* Tiempo medio de llegada de clientes */
	Ts = 4.2;	/* Tiempo medio de servicio del servidor */

#define  MAX(x,y)  ( ((x)>(y)) ? (x) : (y) )


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
static bool serve_customer (double *tsal, double *wtime)
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



/* Mete un nuevo cliente en el servidor
 *
 * PRE: ta == tiempo (absoluto) de arribo del cliente al sistema
 *	CASE1: !busy
 *	CASE2:  busy
 *
 * accepted = receive_customer (ta, &busy, &tsal)
 *
 * POS: !accepted	   => servidor lleno, cliente rechazado
 *
 *	 accepted && CASE1 => el servidor estaba vacio, atendimos al cliente
 *			      *tsal contiene el proximo tiempo (abs) de salida
 *
 *	 accepted && CASE2 => el servidor estaba ocupado, cliente puesto en cola
 *			      *tsal no fue modificado
 */
static bool receive_customer (double ta, bool *busy, double *tsal)
{
	bool accepted = false;
	
	if (q_is_full())
	/* Servidor lleno => se descarta al cliente */
		accepted = false;
	
	else {
		q_enqueue (ta);
		
		if (!(*busy)) {
			/* Servidor vacío => se atiende al cliente directamente */
			*tsal = ta + gen_exp (Ts);
			*busy = true;
		}
		/* else: Servidor ocupado => sólo encolabamos al cliente */
	}
	
	return accepted;
}


/** ------------------------------------------------------------------------- *
 ** ### ### ### ### ### RUTINA PRINCIPAL DEL SERVIDOR ### ### ### ### ### ### *
 ** ------------------------------------------------------------------------- */



int main (void)
{
	/* Variables relacionadas con la simulacion */
	double ta = 0.0;	/* Tiempo (absoluto) de arribo del proximo *
				 * cliente al sistema			   */
	double tsal = 0.0;	/* Tiempo (absoluto) de salida del cliente *
				 * que actualmente esta siendo atendido    */
	double wtime = 0.0;	/* Tiempo que estuvo en el sist. un cliente */
	bool busy = false,	/* Servidor ocupado */
	     accepted = false;	/* Último cliente aceptado / descartado */
	unsigned long rejected = 0;
	
	/* Variables relacionadas con los resultados de las simulaciones */
	unsigned int n = 0;	/* # de evento actual */
	double	X = 0.0,	/* media muestral en el paso 'n' */
		S = 0.0;	/* desv. est. muestral en 'n' */
	double sample[N];	/* Bootstrap: valores muestrales */
	double mse = 0.0;	/* Error Cuadrático Medio (ECM) del estimador*/
	
	
	/* Simulamos el arribo de los clientes mientras TIEMPO_ABSOLUTO < 8 */
	while (ta < T) {
		
		/* Tiempo absoluto de arribo del proximo cliente */
		ta += gen_exp (Ta);
		
		if (ta >= T)
			/* Se acabo el horario de recepción de clientes
			 * Esto asegura que en el sub-ciclo que sigue
			 * se atienda a todos los clientes que
			 * actualmente están dentro del servidor */
			ta = DBL_MAX;
			
		while (ta > tsal && busy) {
			/* Ocurre antes la atencion del actual cliente en el
			 * servidor que el arribo del proximo cliente */
			busy = serve_customer (&tsal, &wtime);
			
			/** << Registramos estadísticas >> */
			n++;
			X = media_m (wtime, (double) n);
			S = var_m (wtime, (double) n);
			
			if (n <= N)
				/* Guardamos el valor muestreado */
				sample[n-1] = wtime;
			else {
				/* No hay más espacio para registros */
				fprintf (stderr, "\nERROR: Se acabó el espacio"
						 " para simulaciones\n\n");
				exit (EXIT_FAILURE);
			}
		}
		
		if (ta < T) {
			/* Metemos al cliente en el servidor, si se puede */
			accepted = receive_customer (ta, &busy, &tsal);
			if (!accepted)
				rejected++;
		}
	}
	
	
	/* Calculamos el error cuadrático medio del estimador con bootstrap */
	mse = bootstrap ((double *) sample, n);
	
	/* Informamos los resultados */
	printf ("\nSe registraron %ui arribos de clientes\n"
		"%lu clientes fueron rechazados por falta de espacio\n\n"
	        "El tiempo medio de espera en el sistema fue:   X = %.8f hs\n"
	        "La varianza en los tiempos de espera fue de: S^2 = %.8f hs\n"
	        "EL error cuadrático medio del estimador fue: ECM = %.8f \n\n",
	        n, rejected, X, S*S, mse);
	
	return 0;
}
