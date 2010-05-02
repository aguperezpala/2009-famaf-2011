#include "laundry_aux.h"


/* Funcion que castea el parametro p
* RETURNS:
* 	< 0 	on error
* 	>=0	cc
*/
int get_param(uint p, char **argv)
{
	int result = -1;
	long int n = 0;
	char *err = NULL;
	
	n = strtol(argv[p], &err, 10);
	if (err[0] != '\0') {
		perror("error en el argumento recibido\n");
		return result;
	}
	result = (int) n;
	
	return result;
}
	

/* Funcion que devuelve un arreglo de tamaño N de wm_t
* RETURNS:
*	NULL		on error
* NOTE: genera un NULL al final de la lista para indicar eso
*/
wm_t *create_machines(uint N)
{
	wm_t *result = NULL;
	int i = 0;
	
	result = (wm_t *) calloc(N+1, sizeof(*result));
	if (!result)
		return NULL;
	
	for (i = 0; i < N; i++)
		result[i] = wm_create();
	
	return result;
}

/* Funcion que devuelve un arreglo de tamaño N de mechanics
* RETURNS:
*	NULL		on error
* NOTE: genera un NULL al final de la lista para indicar eso
*/
mechanics_t *create_mechanics(uint N)
{
	mechanics_t *result = NULL;
	
	result = (mechanics_t *) calloc(N+1, sizeof(*result));
	return result;
}

/* Destructor */
wm_t *destroy_machines(wm_t *wm)
{
	int i = 0;
	
	if(!wm)
		return NULL;
	
	while(wm[i] != NULL){
		wm[i] = wm_destroy(wm[i]);
		i++;
	}
	
	free(wm);
	
	return NULL;
}
/* Destructor */
mechanics_t *destroy_mechanics(mechanics_t *m)
{
	int i = 0;
	
	if(!m)
		return NULL;
	
	while(m[i] != NULL){
		m[i] = mechanic_destroy(m[i]);
		i++;
	}
	
	free(m);
	
	return NULL;
}




/* Funcion que reinicializa toda la lavanderia
* REQUIRES:
* 	opm 	!= NULL
* 	bm	!= NULL
* 	sm	!= NULL
* 	m	!= NULL
*/
void reset_laundry(wm_t *opm, wm_t *bm, wm_t *sm, mechanics_t *m)
{
	
	if (!opm || !bm || !sm || !m) {
		printf("reset_laundry error: argumento null\n");
		return;
	}
	
	
	
}


/* Obtenemos las máquinas que se acaban de romper y las sacamos de entre las 
* operativas 
* REQUIRES:
* 	opm 	!= NULL
* 	bm	!= NULL
*	bo	!= NULL
*/
uint get_broken(wm_t *opm, unsigned long time, wm_t *bm, wm_t *bo);

/* Funcion que obtiene la sumatoria de las maquinas que se estan arreglando en
* los diferentes mecanicos (m)
* REQUIRES:
* 	m 	!= NULL
*/
uint get_repairing(mechanics_t *m);

/* Traemos a servicio las maquinas recién reparadas
* REQUIRES:
* 	m 	!= NULL
* 	sm 	!= NULL
*/
void get_from_mechanics (mechanics_t *m, wm_t *sm);

/* Reemplazamos los vacios con las maquinas de servicio 
* REQUIRES:
* 	sm 	!= NULL
* 	om	!= NULL
*/
void bring_to_operation (wm_t *sm, wm_t *om)

/* Llevamos al taller las maquinas recién rotas 
* REQUIRES:
* 	bm 	!= NULL
* 	m	!= NULL
*/
void give_to_mechanics (wm_t *bm, mechanic_t *m);



