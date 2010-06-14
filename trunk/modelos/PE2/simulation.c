#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include "rdg.h"
#include "queue.h"
#include "actividad2.h"

/** Variables globales */

/* # de simulaciones a correr */
#define  SIM  500

/* maximo # permitido de clientes simultaneos en el sistema */
#define  Q  5

double	T = 8.0,	/* Periodo de tiempo de atencion del servidor */
	Ta = 4.0,	/* Tiempo medio de llegada de clientes */
	Ts = 4.5;	/* Tiempo medio de servicio del servidor */

#define  MAX(x,y)  ( ((x)>(y)) ? (x) : (y) )

#ifdef _DEBUG
  #define debug(s,...) printf(s, ##__VA_ARGS__)
#else
  #define debug(s,...)
#endif


/** ------------------------------------------------------------------------- *
 ** ~~~~~~~ FUNCIONES AUXILIARES PARA ATENCION/RECEPCION DE CLIENTES ~~~~~~~~ *
 ** ------------------------------------------------------------------------- */


/* Atiende al cliente que actualmente esta usando al servidor
 *
 * PRE: !q_is_empty(q) ("hay al menos un cliente en el servidor")
 *	*tsal == tiempo (absoluto) de salida del servidor del cliente actual
 *
 * busy = serve_customer (q, &tsal, &wtime)
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
static bool serve_customer (queue_t q, double *tsal, double *wtime)
{
	bool busy = true;
	
	assert (!q_is_empty(q));
	
	/* Sacamos al cliente del servidor y registramos el tiempo total
	 * que estuvo dentro del sistema */
	*wtime = *tsal - q_dequeue (q);
	
	if (! q_is_empty(q) )
		/* Generamos un tiempo (absoluto) de salida del servidor para
		 * el proximo cliente que sera atendido */
		*tsal = q_first (q) + gen_exp (Ts);
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
 * accepted = receive_customer (q, ta, &busy, &tsal)
 *
 * POS: !accepted	   => servidor lleno, cliente rechazado
 *
 *	 accepted && CASE1 => el servidor estaba vacio, atendimos al cliente
 *			      *tsal contiene el proximo tiempo (abs) de salida
 *
 *	 accepted && CASE2 => el servidor estaba ocupado, cliente puesto en cola
 *			      *tsal no fue modificado
 */
static bool receive_customer (queue_t q, double ta, bool *busy, double *tsal)
{
	bool accepted = true;
	
	if (q_is_full(q))
		/* Servidor lleno => se descarta al cliente */
		accepted = false;
	
	else {
		q_enqueue (q, ta);
		
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
	queue_t q = NULL;	/* Cola del servidor */
	double ta = 0.0;	/* Tiempo (absoluto) de arribo del proximo *
				 * cliente al sistema			   */
	double tsal = 0.0;	/* Tiempo (absoluto) de salida del cliente *
				 * que actualmente esta siendo atendido    */
	double wtime = 0.0;	/* Tiempo que estuvo en el sist. un cliente */
	bool busy = false,	/* Servidor ocupado */
	     accepted = false;	/* Último cliente aceptado / descartado */
	double servedTime = 0.0;	/* Tiempo total de atencion en un dia */
	unsigned long served = 0;	/* # total de clientes en un dia */
	
	/* Variables relacionadas con los resultados de las simulaciones */
	unsigned int i = 0;
	double sample[SIM];	/* Cocientes obtenidos de cada dia simulado */
	
	
	/* NOTE: en el primer lugar de la cola 'q' estara el cliente actualmente
	 *	 atendido por el servidor.
	 *	 Los Q-1 lugares restantes son la cola de espera prop. dicha
	 */
	q = q_create (Q);
	
	for (i=0 ; i<SIM ; i++) {
		
		ta = 0.0;
		tsal = 0.0;
		served = 0;
		servedTime = 0.0;
		q_clean (q);
		
		/* Simulamos el arribo de los clientes mientras TIEMPO_ABSOLUTO < T */
		while (ta < T) {
			
			/* Tiempo absoluto de arribo del proximo cliente */
			ta += gen_exp (Ta);
			
			if (ta >= T)
				/* Se acabo el horario de recepción de clientes
				* Esto asegura que en el sub-ciclo que sigue
				* se atienda a todos los clientes que
				* actualmente están dentro del servidor */
				ta = DBL_MAX;
			
			while ((ta >= tsal) && busy) {
				/* Ocurre antes la atencion del actual cliente en el
				* servidor que el arribo del proximo cliente */
				busy = serve_customer (q, &tsal, &wtime);
				servedTime += wtime;
			}
			
			if (ta < T) {
				/* Metemos al cliente en el servidor, si se puede */
				accepted = receive_customer (q, ta, &busy, &tsal);
				if (accepted)
					served++;
			}
		}
		/* Registramos el cociente obtenido en este dia */
		debug ("sim # %u\tservedTime = %.4f\tserved = %lu\n",
			i, servedTime, served);
		sample[i] = servedTime / (double) served;
	}
	
	debug ("%s","\nSample:");
	for (i=0 ; i<SIM ; i++)
		debug ("\t%.8f\n", sample[i]);
	debug ("%s","\n");
/*
	printf ("\nMedia:\t%.8f\n", act2_get_media  (sample, SIM));
	printf ("\nVar:\t%.8f\n", act2_get_varianza (sample, SIM));
	printf ("\nMin:\t%.8f\n", act2_get_min (sample, SIM));
	printf ("\nMax:\t%.8f\n", act2_get_max (sample, SIM));
	printf ("\nMediana:\t%.8f\n\n", act2_get_mediana (sample, SIM));
	printf ("\nSkewness:\t%.8f\n\n", act2_get_skewness (sample, SIM));
*/	
	q = q_destroy (q);
	
	return 0;
}
