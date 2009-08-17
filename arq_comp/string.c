#include "string.h"


struct _string {
	unsigned int size;	/* el tamaño del string */
	unsigned int p;		/* el puntero del visor */
	char * data;		/* los datos */
};