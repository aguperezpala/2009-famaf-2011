#include <stdlib.h>
#include <assert.h>
#include "queue.h"




struct _queue {
	double *q;		/* Cola */
	unsigned int n;		/* Tamanio del arreglo */
	unsigned int f;	/* Primero elemento de la cola */
	unsigned int l;	/* Ultimo elemento de la cola */
};


queue_t q_create (unsigned int n)
{
	queue_t q = NULL;
	
	q = (queue_t) calloc (1, sizeof (struct _queue));
	assert (q != NULL);
	
	q->q = (double *) calloc (n, sizeof (double));
	assert (q->q != NULL);
	q->n = n;
	q->f = 0;
	q->l = 0;
	
	return q;
}



/* PRE: q != NULL */
queue_t q_destroy (queue_t q)
{
	assert (q != NULL);
	
	free (q->q);	q->q = NULL;
	free (q);	q = NULL;
	
	return q;
}



/* Indica si la cola de espera esta actualmente vacia
 * PRE: q != NULL
 */
bool q_is_empty (queue_t q) { assert (q != NULL); return q->f == q->l; }



/* Indica si la cola de espera esta actualmente llena
 * PRE: q != NULL
 */
bool q_is_full (queue_t q)
{
	assert (q != NULL);
	return (q->f == (q->l + 1) % q->n);
}



/* Mete un nuevo cliente (con tiempo de arribo absoluto == 'ta')
 * al final de la cola global 'q'
 * PRE: ! q_is_full (q)
 */
void q_enqueue (queue_t q, double ta)
{
	assert (!q_is_full(q));
	
	q->q[q->l] = ta;
	q->l = (q->l + 1) % q->n;
	
	return;
}


/* Devuelve el tiempo de arribo absoluto al sistema
 * del cliente que actualmente esta siendo atendido en el servidor
 *
 * PRE: ! q_is_empty (q)
 */
double q_first (queue_t q) { assert(!q_is_empty(q)); return q->q[q->f]; }


/* Quita el primer elemento de la cola global 'q',
 * y devuelve su tiempo absoluto de arribo al sistema 'ta'
 * PRE: ! q_is_empty ()
 */
double q_dequeue (queue_t q)
{
	assert(!q_is_empty(q));
	
	q->f = (q->f + 1) % q->n;
	
	if (q->f == 0)
		return q->q[q->n-1];
	else
		return q->q[q->f-1];
}


/* Borra todos los datos de la cola y la deja vacia
 * PRE: q != NULL
 * POS: q_is_empty(q)
 */
void q_clean (queue_t q) { assert (q != NULL); q->f = q->l = 0; return; }
