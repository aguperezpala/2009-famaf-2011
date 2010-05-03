#include "washingm.h"


/* Funcion constructora
* RETURNS;
* 	NULL		on error
*	wm		if success
*/
wm_t wm_create(void)
{
	wm_t result = NULL;
	
	result = (wm_t) malloc(sizeof(*result));
	
	return result;
}

/* Funcion destructora 
* REQUIRES:
* 	wm != NULL
*/
wm_t wm_destroy(wm_t wm)
{
	if (wm)
		free(wm);
	
	return NULL;
}
