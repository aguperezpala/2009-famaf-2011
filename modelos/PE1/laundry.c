#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <assert.h>
#include <glib.h>
#include "laundry.h"
#include "rg.h"
#include "mechanic.h"
#include "washingm.h"



#define INV assert(l != NULL); \
	    assert(l->all_machines != NULL); \
	    assert(l->op_machines != NULL); \
	    assert(l->serv_machines != NULL); \
	    assert(l->m != NULL);



struct _laundry {
	/* Máquinas lavadoras */
	wm_t *all_machines;	/* Todas las máquinas */
	GQueue *op_machines;	/* Referencia a las maquinas operativas */
	GQueue *serv_machines;	/* Referencia a las maquinas de servicio */
	/* Mecánicos */
	mechanic_t *m;		/* Mecanicos del taller de reparación */
	int next_mech;		/* Próximo mecánico a sacar una máquina reparada */
	/* Parámetros */
	unsigned int N; 	/* # total de maquinas operativas */
	unsigned int S; 	/* # total de maquinas de servicio */
	unsigned int M; 	/* # de mecanicos */
	double Tf;		/* Tiempo medio de fallo de una lavadora */
	double Tr;		/* Tiempo medio de raparación de una lavadora */
};



/** ------------------------------------------------------------------------- *
 ** ~~~~~~~~~~~~~~~~~~~~~ ###  FUNCIONES PRIVADAS  ### ~~~~~~~~~~~~~~~~~~~~~~ *
 ** ------------------------------------------------------------------------- */



/* Función auxiliar para comparar los futuros tiempos de ruptura
 * de dos máquinas lavadoras m1 y m2 (de tipo wm_t)
 * Se utiliza para mantener ordenada la cola de máquinas lavadoras operativas.
 * PRE:
 *	m1 != NULL
 *	m2 != NULL
 *	dummy == NULL
 * RETURNS:
 *	-1 <= si m1 se romperá antes que m2
 *	 1 <= si m2 se romperá antes que m1
 *	 0 <= si m1 y m2 se romperán en el mismo instante
 */
static int machine_breakage_order (const void *m1, const void *m2, void *dummy)
{
	assert (m1 != NULL);
	assert (m2 != NULL);
	assert (dummy == NULL);
	
	/* El compilador se queja porque para acceder a m1 y m2 tengo que
	 * castearlos a (wm_t), y eso no respeta el calificador *const* */
	if (((wm_t)m1)->nbt < ((wm_t)m2)->nbt)
		return -1;
	else if (((wm_t)m1)->nbt > ((wm_t)m2)->nbt)
		return 1;
	else
		return 0;
}



/* Función auxiliar para actualizar los tiempos de ruptura de las lavadoras
 * A cada lavadora de la lista se le disminuirá su nbt en "time" unidades
 * PRE:
 *	machine != NULL
 *	time != NULL
 */
static void elapse_time (void *machine, void *time)
{
	assert (machine != NULL);
	assert (time != NULL);
	((wm_t)machine)->nbt -= *((double *) time);
	return;
}



/* Devuelve el próximo tiempo de ruptura (RELATIVO) de entre las máquinas
 * lavadoras operativas.
 * PRE:
 *	INV
 * POS:
 *	El resultado devuelto es el tiempo de ruptura más cercano en el tiempo
 */
static double laundry_get_nbt (laundry_t l)
{
	wm_t machine = NULL;
	
	INV
	
	/* Sabemos que la cola de máquinas operativas está ordenada
	 * La máquina con menor tiempo de ruptura futuro está primera */
	machine = g_queue_peek_head (l->op_machines);
	
	return machine->nbt;
}



/* Devuelve el tiempo (RELATIVO) futuro en el que saldrá la próxima máquina
 * de taller, de entre todos los mecánicos de la lavandería.
 * PRE:
 *	INV
 * POS:
 *	El resultado devuelto es el próximo tiempo de reparación
 *	(el más cercano en el tiempo)
 */
static double laundry_get_nr (laundry_t l)
{
	unsigned int i = 0;
	double	nr = DBL_MAX,	/* next repair time */
		mnr = 0.0;	/* "this mechanic" next repair time */
	
	INV
	
	for (i=0 ; i<l->M ; i++) {
		mnr = mechanic_get_rrt (l->m[i]);
		
		if (mnr > 0 && mnr < nr) {
			nr = mnr;	  /* Mínimo tiempo futuro de reparac. */
			l->next_mech = i; /* Mecánico que lo realiza */
		}
	}
	
	return nr;
}



/* Hace que transcurran "elapsed" unidades de tiempo en la lavandería
 * Es decir, disminuye en "elapsed" unidades todos los tiempos del sistema
 *
 * PRE:
 *	INV
 */
static void laundry_update_time (laundry_t l, double elapsed)
{
	unsigned int i = 0;
	
	INV
	
	/* Actualizamos las lavadoras operativas... */
	g_queue_foreach (l->op_machines, elapse_time, &elapsed);
	
	/* ...y los mecánicos */
	for (i=0 ; i<l->M ; i++) {
		/* Si el mecánico estaba reparando => actualizamos su tiempo */
		if (0 < mechanic_get_rrt(l->m[i]))
			mechanic_elapse_time (l->m[i], elapsed);
	}
		
	return;
}



/* De todo el grupo de mecánicos del taller de la lavandería
 * busca al que tiene menor tiempo futuro de reparación.
 * Le hace reparar su lavadora, se la quita, y le da la siguiente de su cola
 * A la lavadora reparada la mete dentro de la lista de máquinas de servicio
 *
 * PRE:
 *	INV
 * RETURNS:
 *	 0 , si se pudo hacer todo bien
 *	-1 , si no se encontró al mecánico
 */
static int laundry_bring_from_repair (laundry_t l)
{
	INV
	
	if (l->next_mech < 0) {
		/* No sabemos cual es el siguiente mecánico => lo buscamos */
		double dummy = 0.0;
		dummy = laundry_get_nr (l);
	}
	
	if (l->next_mech < 0 || l->next_mech >= (int) l->M)
		return -1;
	else {
		wm_t machine = NULL;
		mechanic_t m = l->m[l->next_mech];
		
		/* Le quitamos la máquina al mecánico... */
		machine = mechanic_get_rm (m);
		if (machine == NULL)
			return -1;
		else
			l->next_mech = -1;
		
		/* ...y la ponemos de servicio */
		g_queue_push_tail (l->serv_machines, machine);
	}
	
	return 0;
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
	mechanic_repair_machine (l->m[pos], machine);
	
	return;
}



/* Buscamos la próxima lavadora operativa que se está por romper.
 * La llevamos al taller mecánico y la reemplazamos con una máquina de servicio
 * 
 * PRE:
 *	INV
 *	"aún hay máquinas de servicio"
 */
static void laundry_send_to_repair (laundry_t l)
{
	wm_t machine = NULL;
	
	INV
	assert (!g_queue_is_empty(l->serv_machines));
	
	/* Obtenemos la lavadora... */
	machine = (wm_t) g_queue_pop_head (l->op_machines);
	
	/* ...la enviamos al taller... */
	repair_machine (l, machine);
	
	/* ...y la reemplazamos con una de servicio */
	machine = (wm_t) g_queue_pop_head (l->serv_machines);
	machine->nbt = rg_gen_exp (1.0/l->Tf);
	
	/* Al meterla entre las operativas mantenemos el orden */
	g_queue_insert_sorted (l->op_machines, machine, machine_breakage_order, NULL);
}



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



/* Destructor de un arreglo de maquinas. Se detiene al primer NULL hallado
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



/* Destructor de un arreglo de mecanicos. Se detiene al primer NULL hallado
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
 ** ~~~~~~~~~~~~~~~~~~~~~ ###  FUNCIONES PÚBLICAS  ### ~~~~~~~~~~~~~~~~~~~~~~ *
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
	l->op_machines	 = g_queue_new();
	l->serv_machines = g_queue_new();
	l->m = create_mechanics (Nmech, Tr);
	l->next_mech = -1;
	
	/* Al término de la creación aseguramos el invariante de la lavandería */
	INV
	
	return l;
}



/* Destructor del TAD
 * PRE:
 *	l != NULL
 *	l fue creada con laundry_create()
 * POS:
 *	todos los recursos de memoria de 'l' fueron liberados
 */
laundry_t laundry_destroy (laundry_t l)
{
	INV
	
	l->all_machines = destroy_machines (l->all_machines);
	g_queue_clear (l->op_machines);   l->op_machines   = NULL;
	g_queue_clear (l->serv_machines); l->serv_machines = NULL;
	l->m = destroy_mechanics (l->m);
	
	free(l); l = NULL;

	return l;
}



/* Simulación de una operación de la lavandería
 * desde el tiempo inicial = 0 hasta el primer fallo del sistema
 * PRE:
 *	l != NULL
 *	l fue creada con laundry_create()
 * POS:
 *	Devuelve el tiempo transcurrido hasta el 1º fallo del sistema
 */
double laundry_simulation (laundry_t l)
{
	bool	fail = false;	/* Fallo del sistema */
	double	nbt  = 0.0,	/* Prox. ruptura de entre todas las maq. op. */
		nr   = 0.0,	/* Prox. reparación de entre todos los mecánicos */
		time = 0.0;	/* Tiempo absoluto del sistema */
	int err = 0;
	
	INV
	laundry_reset (l);
	time = 0.0;
	
	while (!fail) {
		/* El tiempo de la lavandería aumenta de a eventos.
		 * El próximo evento, ya sea una ruptura o una reparación,
		 * hace que el tiempo se adelante hasta él
		 */
		nbt = laundry_get_nbt (l);	/* Laundry next breaking time */
		nr  = laundry_get_nr  (l);	/* Laundry next repair time */
		
		if (nr <= nbt) {
			/* Una reparación es el próximo evento:
			 * reflejamos el transcurso del tiempo */
			laundry_update_time (l, nr);
			time += nr;
			/* Traemos la máquina reparada a servicio */
			err = laundry_bring_from_repair (l);
			if (err) {
				fprintf (stderr, "Error al reparar lavadora\n");
				exit (EXIT_FAILURE);
			}
		} else {
			/* Una ruptura es el próximo evento:
			 * reflejamos el transcurso del tiempo */
			laundry_update_time (l, nbt);
			time += nbt;
			
			/* Llevamos la máquina rota al mecánico
			 * y traemos a operación una máquina de servicio */
			if (g_queue_is_empty (l->serv_machines))
				/** FALLO DEL SISTEMA */
				fail = true;
			else
				laundry_send_to_repair (l);
		}
	}
	
	return time;	/* time == system failure time */
}



/* Funcion que reseta toda la estructura de laundry, dejandola en 0km.
 * REQUIRES:
 *	l != NULL
 *	l fue creada con laundry_create()
 */
void laundry_reset (laundry_t l)
{
	unsigned int i = 0;
	wm_t machine = NULL;
	
	INV
	
	/* Limpiamos los conjuntos de lavadoras */
	g_queue_clear (l->op_machines);
	g_queue_clear (l->serv_machines);
	/* Limpiamos el taller mecanico */
	for (i = 0; i < l->M; i++)
		mechanic_reinitialize(l->m[i]);
	
	/* Distribuimos N lavadoras en el arreglo de maquinas operativas */
	for (i = 0; i < l->N; i++) {
		machine = l->all_machines[i];
		/* Al ponerla en operación le damos un tiempo de ruptura futuro */
		machine->nbt = rg_gen_exp (1.0/l->Tf);
		g_queue_push_tail (l->op_machines, machine);
	}
	/* Ordenamos de forma creciente las máquinas lavadoras operativas según
	 * según sus tiempos de ruptura futuros
	 * Así "próxima máquina a romperse" == head (l->op_machines)
	 */
	g_queue_sort (l->op_machines, &machine_breakage_order, NULL);
	
	/* Distribuimos S lavadoras en el arreglo de maquinas de servicio */
	for (i = l->N; i < l->S + l->N; i++)
		g_queue_push_tail (l->serv_machines, l->all_machines[i]);
	
	return;
}
