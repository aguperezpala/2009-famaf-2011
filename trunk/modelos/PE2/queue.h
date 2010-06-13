#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>


typedef struct _queue *queue_t;



queue_t q_create (unsigned int n);



/* PRE: q != NULL */
queue_t q_destroy (queue_t q);



/* Indica si la cola de espera esta actualmente llena
 * PRE: q != NULL
 */
bool q_is_full (queue_t q);



/* Indica si la cola de espera esta actualmente llena
 * PRE: q != NULL
 */
bool q_is_full (queue_t q);



/* Mete un nuevo cliente (con tiempo de arribo absoluto == 'ta')
 * al final de la cola global 'q'
 * PRE: ! q_is_full (q)
 */
void q_enqueue (queue_t q, double ta);



/* Devuelve el tiempo de arribo absoluto al sistema
 * del cliente que actualmente esta siendo atendido en el servidor
 *
 * PRE: ! q_is_empty (q)
 */
double q_first (queue_t q);



/* Quita el primer elemento de la cola global 'q',
 * y devuelve su tiempo absoluto de arribo al sistema 'ta'
 * PRE: ! q_is_empty ()
 */
double q_dequeue (queue_t q);


#endif