#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include "laundry.h"
#include "rg.h"
#include "mechanic.h"
#include "washingm.h"



#define INV assert(l != NULL); \
	    assert(l->all_machines != NULL); \
	    assert(l->op_machines != NULL); \
	    assert(l->serv_machines != NULL); \
	    assert(l->m != NULL); \
	    assert(!l->failure);



struct _laundry {
	/* Máquinas lavadoras */
	wm_t *all_machines;	/* Todas las máquinas */
	wm_t *op_machines;	/* Referencia a las maquinas operativas */
	wm_t *serv_machines;	/* Referencia a las maquinas de servicio */
	/* Mecanicos */
	mechanic_t *m;		/* Mecanicos del taller de reparación */
	/* Cantidades */
	unsigned int N; 	/* # total de maquinas operativas */
	unsigned int S; 	/* # total de maquinas de servicio */
	unsigned int M; 	/* # de mecanicos */
	double Tf;		/* Tiempo medio de fallo de una lavadora */
	double Tr;	/* Tiempo medio de raparación de una lavadora */
	double time;		/* Tiempo operativo continuo de la lavandería */
	/* Auxiliares */
	int s;			/* Última máquina de servicio lista + 1 */
	bool failure;		/* ¿Hay fallo de sistema? */
};



/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~~~~~~~~~~~~     FUNCIONES PRIVADAS     ~~~~~~~~~~~~~~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */

/* Funcion que agrega una maquina en un arreglo en la ultima posicion libre
 * (osea NULL)
 * REQUIRES:
 * 	wms != NULL
 * 	wm  != NULL
 * 	haber lugar en el arreglo wms
 */
static void insert_wm_wms(wm_t *wms, wm_t wm, int size)
{
	int i = 0;
	
	assert(wms != NULL);
	assert(wm != NULL);
	
	while (i < size && wms[i] != NULL)
		i++;
	if (i >= size)
		/* intentando agregar donde no se puede */
		assert(false);
	wms[i] = wm;
	
	return;
}

/* Funcion que devuelve la ultima wm de un arreglo, o NULL si no hay.
 * REQUIRES:
 * 	wms != NULL
 * RETURNS:
 * 	NULL		if no wm
 * 	wm		otherwise
 */
static wm_t get_wm_wms(wm_t *wms, int size)
{
	int i = 0;
	wm_t result = NULL;
	
	assert(wms != NULL);
	
	while(i < size && wms[i] != NULL)
		i++;
	i--;
	/* hay maquinas? */
	if (i < 0)
		/* NO hay */
		return NULL;
	
	/* si hay una maquina => la devolvemos */
	result = wms[i];
	wms[i] = NULL;
	
	return result;
}

/* Traemos a servicio las maquinas recién reparadas */
static void get_from_mechanics (laundry_t l)
{
	unsigned int i = 0;/*, last = 0;*/
	wm_t RMachine = NULL;	/* Lavadora reparada */
	
	INV
	
	/*last = l->s;*/
	for (i=0 ; i < l->M ; i++) {
		/* Le preguntamos al mecánico si ya tiene una lavadora lista */
		RMachine = mechanic_get_rm (l->m[i], l->time);
		if (RMachine != NULL) {
			/* Reparó una => la pasamos a servicio 
			l->serv_machines[last] = RMachine;
			last++;
			*/
			/*! FIXME: estoy usando una version lenta pero segura */
			insert_wm_wms(l->serv_machines, RMachine, l->S);
		}
	}
	/*! FIXME: no entiendo porque mierda CON esta linea (l->s = last) se
	 * caga clavando todo bolo, osea, no termina mas, queda infinitamente
	 * dando vueltas... Yo se la agregue, porque me parece que si obtenemos
	 * una maquina del mecanico y la colocamos en la de servicio, entonces
	 * teniamos que aumentar l->s, si no siempre va a ir disminuyendo esto
	 * y terminaria antes... el tema es que eso hace que no termine, 
	 * sera que no es computable? XD
	 */
	/*l->s = last;*/
	return;
}


/* Mete en el taller la máquina lavadora pasada como 2º argumento,
 * entregándosela al mecánico con cola de reparación más corta
 * PRE:
 *	INV
 *	machine != NULL
 * POS:
 *	la lavadora fue puesta en la cola de reparación (o entregada
 *	directamente) del mecánico menos ocupado
 */
static void repair_machine (laundry_t l, wm_t machine)
{
	unsigned int i = 0;
	int pos = -1, min = INT_MAX, mech_q_len = 0;
	
	INV
	assert (machine != NULL);
	
	/* Buscamos al mecánico con cola de reparación más corta... */
	for (i=0 ; i<l->M ; i++) {
		mech_q_len = mechanic_get_n_rmachines (l->m[i]);
		if (min > mech_q_len) {
			min = mech_q_len;
			pos = i;
		}
	}
	/* ...y le damos la lavadora para que la arregle */
	mechanic_repair_machine (l->m[pos], machine, l->time);
	
	return;
}

/* Lleva al taller las lavadoras operativas que acaban de romperse
* PRE:
*	INV
* POS:
*	Toda máquina referenciada en la lista de operativas de la lavandería
*	tiene tiempo de ruptura > tiempo actual. En código:
*	(l->op_machines[i]).nbt > l->time  |
*	ó sino l->op_machines[i] == NULL   | para todo i
*/
static void give_to_mechanics (laundry_t l)
{
	unsigned int i = 0;
	wm_t BMachine = NULL;	/* Lavadora rota */
	
	INV
	
	/* Quitamos las lavadoras que se rompieron este mes */
	for (i=0 ; i < l->N ; i++) {
		BMachine = l->op_machines[i]->nbt == l->time ? \
		l->op_machines[i] : NULL;
		/* Si la i-ésima máquina está rota... */
		if ( BMachine != NULL ) {
			/* ...la sacamos de entre las operativas... */
			l->op_machines[i] = NULL;
			/* ...y la mandamos a arreglar */
			repair_machine (l, BMachine);
		}
	}
	
	return;
}

/* Funcion que actualiza el tiempo donde ocurrira el proximo evento
 * REQUIRES:
 * 	l != NULL
 * POS:
 * 	el l->time = al tiempo en el que ocurria el proximo evento.
 */
static void incrase_time(laundry_t l)
{
	unsigned int i = 0;
	double min = 0, auxT = 0;
	
	INV
	
	/* debemos buscar el minimo tiempo entre la proxima reparacion de una
	 * maquina o la proxima rotura de una maquina. Buscamos primero en los
	 * mecanicos */
	min = l->op_machines[0]->nbt; /* sabemos que esto existe por INV */
	
	for (i=0 ; i<l->M ; i++) {
		auxT = mechanic_get_rrt(l->m[i]);
		/* debemos verificar que el tiempo del mecanico sea > 0 */
		if (auxT < 0)
			continue;
		if (min > auxT) {
			min = auxT;
		}
	}
	/* ahora debemos buscar entre las proximas "roturas de las maquinas,
	 * teniendo en cuenta el mismo minimo */
	for (i = 0; i < l->N; i++) {
		auxT = l->op_machines[i]->nbt;
		if (min > auxT) {
			min = auxT;
		}
	}
	/* en este punto tenemos en min el tiempo en el que va a ocurrir
	 * el proximo evento => actualizamos el tiempo al proximo evento
	 */
	l->time = min;
	
	/*! NOTE: recordemos que estamos trabajando con tiempos ABSOLUTOS,
	 * 	 si no esto no andaria NADA */
	
	return;
}

/* Reemplazamos los espacios vacíos entre las máquinas operativas
 * con las máquinas lavadoras de servicio 
 * PRE:
 *	INV
 * POS:
 *	Toda máquina referenciada en la lista de operativas de la lavandería
 *	tiene tiempo de ruptura > tiempo actual, ó sino la lavandería acaba
 *	de incurrir en un fallo de sistema
 *	
 * NOTE: ÉSTA ES LA FUNCIÓN QUE REVELA LOS FALLOS DE SISTEMA
 */
static void bring_to_operation (laundry_t l)
{
	unsigned int i = 0;
	int nbt = 0;
	wm_t sm = NULL;
	
	INV
	
	for (i=0 ; i < l->N ; i++) {
		/* Si esta lavadora estaba rota y la llevaron al taller... */
		if (l->op_machines[i] == NULL) {
			/*! FIXME: usando version muy lenta, pero segura */
			sm = get_wm_wms(l->serv_machines, l->S);
			if (sm != NULL) {
				/* ...la reemplazamos por una de servicio... */
				/*l->s--;*/
				l->op_machines[i] = sm;
				/*! FIXME: faltaba esto, es necesario? */
				/*l->serv_machines[l->s] = NULL;*/
				/* ...y le damos un nuevo tiempo de ruptura */
				nbt = rg_gen_exp((double) 1.0/l->Tf);
				l->op_machines[i]->nbt = l->time + nbt;
			} else {
			/* Si ya no hay en servicio => FALLO DEL SISTEMA */
				l->failure = true;
				break;
			}
		}
	}
	return;
}



/*static void wash (void) { printf ("washing washing...\n"); return; }*/



/* Funcion que devuelve un arreglo de tamaño N de wm_t
* RETURNS:
*	NULL		on error
* NOTE: genera un NULL al final de la lista para indicar eso
*/
static wm_t *create_machines(unsigned int N)
{
	wm_t *result = NULL;
	unsigned int i = 0;
	
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
static mechanic_t *create_mechanics(unsigned int N, double tr)
{
	mechanic_t *result = NULL;
	unsigned int i = 0;
	
	result = (mechanic_t *) calloc(N+1, sizeof(*result));
	if (!result)
		return NULL;
	for (i = 0; i < N; i++)
		result[i] = mechanic_create(tr);
	
	return result;
}

/* Destructor de un arreglo de maquinas
* REQUIRES:
*  	wm != NULL
*/
static wm_t *destroy_machines(wm_t *wm)
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
/* Destructor de un arreglo de mecanicos
 * REQUIRES:
 *  	m != NULL
 */
static mechanic_t *destroy_mechanics(mechanic_t *m)
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





/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~~~~~~~~~~~~     FUNCIONES PÚBLICAS     ~~~~~~~~~~~~~~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */



/* Creador de TAD. Deberá luego ser liberado con el destructor aquí provisto
 * Especificación de los parámetros:
 *	Nop:	# de máquinas operativas
 *	Nserv:	# de máquinas de servicio
 *	Nmech:	# de mecánicos en el taller de reparación
 *	Tf:	tiempo medio de fallo de una lavadora operativa
 *	Tr	tiempo medio de reparación de un mecánico
 */
laundry_t laundry_create (unsigned int Nop, unsigned int Nserv, 
			   unsigned int Nmech, double Tf, double Tr)
{
	laundry_t l = NULL;
	
	l = (laundry_t) malloc (sizeof (struct _laundry));
	assert (l != NULL);
	
	l->N  = Nop;
	l->S  = Nserv;
	l->M  = Nmech;
	l->Tf = Tf;
	l->Tr = Tr;
	l->all_machines  = create_machines (Nop + Nserv);
	l->op_machines	 = (wm_t *) calloc (Nop,   sizeof(wm_t));
	l->serv_machines = (wm_t *) calloc (Nserv, sizeof(wm_t));
	l->s = Nserv;
	l->m = create_mechanics (Nmech, Tr);
	l->time = 0;
	l->failure = false;
	
	/* Al término de la creación aseguramos el invariante de la lavandería */
	INV
	
	return l;
}



/* Destructor del TAD
 * PRE:
 *	l != NULL
 * POS:
 *	todos los recursos de memoria de 'l' fueron liberados
 */
laundry_t laundry_destroy (laundry_t l)
{
	assert (l != NULL);
	
	l->all_machines = destroy_machines (l->all_machines);
	free (l->op_machines);   l->op_machines   = NULL;
	free (l->serv_machines); l->serv_machines = NULL;
	l->m = destroy_mechanics (l->m);
	
	free(l); l = NULL;

	return l;
}



/* Lleva a cabo la operación normal de la lavandería durante un mes, es decir:
 * 1: llevamos a servicio las máquinas que salgan de taller
 * 2: llevamos a taller las máquinas operativas que se rompieron
 * 3: reemplazamos los vacíos operativos con máquinas de servicio
 * 4: lavamos ropa
 * PRE:
 *	INV
 */
void laundry_wash_clothes (laundry_t l)
{
	INV
	
	/* Algoritmos: 
	 * 1) Traemos del mecanico las maquinas reparadas
	 * 2) Mandamos al mecanico las maquinas rotas.
	 * 3) Reponemos las maquinas rotas con las de servicio y le seteamos
	 *    un tiempo de vida util.
	 * 4) Verificamos si hubo fallo del sistema (si hubo no actualizamos
	 *    ningun tiempo y salimos)
	 * 5) Buscamos el menor tiempo para que suceda el proximo evento {se
	 *    reparo una maquina | se rompio una maquina} y actualizamos
	 *    el tiempo actual al tiempo del proximo evento.
	 * NOTE: para que esto sea eficiente deberiamos tener que almacenar
	 * si el evento proximo es de rotura de maquina, o de reparacion, o de
	 * ambos (y guardar la posicion o el mecanico que producira el evento)
	 */
	
	/* Traemos a servicio las maquinas recién reparadas */
	get_from_mechanics (l);
	/* Llevamos al taller las maquinas recién rotas */
	give_to_mechanics (l);
	/* Reemplazamos los vacios con las maquinas de servicio */
	bring_to_operation (l);
	
	/* ahora vamos a ver si actualizamos el tiempo o si no hacemos nada */
	if(!l->failure)
		incrase_time(l);
	
#ifdef _WASH
	/* Lavamos la ropa */
	wash();
#endif
	return;
}




/* Indica si la lavandería dejó de ser operativa, ie: si tiene menos máquinas
 * lavadoras en operación de las que debería
 * PRE:
 *	l != NULL
 */
bool laundry_failure (laundry_t l) { assert (l!=NULL); return l->failure; }



/* Devuelve el tiempo en el que ocurrió el último fallo del sistema
 * REQUIRES:
 *	l != NULL
 * RETURNS
 * 	ftime >=0 , si el sistema falló tras "ftime" meses de operación
 * 	ftime < 0 , si el sistema aún no falló
 */
double laundry_get_failure_time (laundry_t l)
{
	assert (l!=NULL);
	
	if (laundry_failure (l))
		return l->time;
	else
		return -1;
}


/* Funcion que reseta toda la estructura de laundry, dejandola en 0km.
* REQUIRES:
* 	l != NULL
*/
void laundry_reset(laundry_t l)
{
	unsigned int i = 0;
	unsigned int j = 0;
	
	assert(l != NULL);
	
	/* reinicializamos las variables internas */
	l->time = 0;
	l->failure = false;
	memset(l->op_machines, 0, l->N * sizeof(*l->op_machines));
	memset(l->serv_machines, 0, l->S * sizeof(*l->serv_machines));
	/* reinicializamos los mecanicos */
	for (i = 0; i < l->M; i++)
		mechanic_reinitialize(l->m[i]);
	
	/* ahora distribuimos N maquinas en el arreglo de maquinas operativas
	 * y S maquinas en el arreglo de maquinas de servicio */
	for (i = 0; i < l->N; i++) {
		l->op_machines[i] = l->all_machines[i];
		l->op_machines[i]->nbt = rg_gen_exp((double) 1.0/l->Tf) + l->time;
	}
	for (j = i; j < l->S + l->N; j++)
		l->serv_machines[j-i] = l->all_machines[j];
	
	l->s = l->S;
}