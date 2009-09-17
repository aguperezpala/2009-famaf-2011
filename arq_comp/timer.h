/* * * * * * * * * * * * * * * * * * * * * * * * * 
 * TAD Timer trucho				 *
 *						 *
 * Son básicamente dos enteros wrappeados: un	 *
 * incrementador y un consultor de timeout.	 *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef TIMER_H
#define TIMER_H

typedef struct __timer timer;


/** ~~~ Constructores/destructores ~~~ **/

/* Creación de un timer vacío con tiempo de expiración 'timeout'
 *
 * PRE: timeout > 0
 *    t = setup_timer (timeout)
 * POS: t != NULL
 */
timer *setup_timer (unsigned int timeout);


/* Borra la estructura
 *
 * PRE: t != NULL
 *    t = stop_timer (t)
 * POS: t == NULL && "no leaks"
 */
timer *stop_timer (timer *t);


/** ~~~ Consultores ~~~ **/

/* Indica si el contador del timer alcanzó el tiempo de expiración
 *
 * PRE: t != NULL
 *    b = timeout_timer (t)
 * POS:  b => el timer alcanzó el tiempo seteado con start_timer
 *	¬b => aún no se alcanzó ese tiempo
 */
bool timeout_timer (timer *t);


/** ~~~ Modificadores ~~~ **/

/* Función de inicialización. Pone el contador en 0.
 *
 * PRE: t != NULL
 *    start_timer (t)
 * POS: !timeout_timer (t)
 */
void start_timer (timer *t);


/* Aumenta en 1 el contador del timer
 *
 * PRE: t != NULL
 *    inc_timer (t)
 * POS: eso que dice ahí arriba
 */
void inc_timer (timer *t);


#endif
