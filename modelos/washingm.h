#ifndef WASHING_MACHINE_H
#define WASHING_MACHINE_H

#include <stdio.h>
#include <stdlib.h>

/*! Vamos hacer este tad transparente para accederlo directamente */

struct _wm {
	int nbt	/* Tiempo absoluto hasta la proxima ruptura */
};

typedef struct _wm * wm_t;


/* Funcion constructora
 * RETURNS;
 * 	NULL		on error
 *	wm		if success
 */
wm_t wm_create(void);

/* Funcion destructora 
 * REQUIRES:
 * 	wm != NULL
 */
wm_t wm_destroy(wm_t wm);



#endif
