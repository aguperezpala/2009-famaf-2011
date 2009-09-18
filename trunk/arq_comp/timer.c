/* * * * * * * * * * * * * * * * * * * * * * * * * 
 * TAD Timer trucho				 *
 * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include "timer.h"
/* FIXME: implementar el uso de la macro ASSERT cuando ésta sea implementada
#include "assert.h" */


struct __timer {
	unsigned int to; /* Timeout */
	unsigned int ct; /* Counter */
};



/** ~~~ Constructores/destructores ~~~ **/

/* Creación de un timer vacío con tiempo de expiración 'timeout'
 *
 * PRE: timeout > 0
 *    t = setup_timer (timeout)
 * POS: t != NULL
 */
timer *setup_timer (unsigned int timeout)
{
	timer *t = NULL;
	
	if (timeout > 0)
		t = (timer *) calloc (1, sizeof (struct __timer));
	
	if (t != NULL)	
		t->to = timeout;
	
	return t;
}


/* Borra la estructura
 *
 * PRE: t != NULL
 *    t = stop_timer (t)
 * POS: t == NULL && "no leaks"
 */
timer *stop_timer (timer *t)
{
	if (t != NULL)
		free (t);
	t = NULL;
	
	return t;
}


/** ~~~ Consultores ~~~ **/

/* Indica si el contador del timer alcanzó el tiempo de expiración
 *
 * PRE: t != NULL
 *    b = timeout_timer (t)
 * POS: 1 => el timer alcanzó el tiempo seteado con start_timer
 *	0 => aún no se alcanzó ese tiempo
 */
int timeout_timer (timer *t)
{
	if (t != NULL)
		return t->to <= t->ct;
	else
		return 0;
}


/** ~~~ Modificadores ~~~ **/

/* Función de inicialización. Pone el contador en 0.
 *
 * PRE: t != NULL
 *    start_timer (t)
 * POS: !timeout_timer (t)
 */
void start_timer (timer *t)
{
	if (t != NULL)
		t->ct = 0;
}


/* Aumenta en 1 el contador del timer
 *
 * PRE: t != NULL
 *    inc_timer (t)
 * POS: eso que dice ahí arriba
 */
void inc_timer (timer *t)
{
	if (t != NULL)
		t->ct++;
}
