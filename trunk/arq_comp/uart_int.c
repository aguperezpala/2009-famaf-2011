/**
*	Centronic interrupt program example 1 (in C).
*
*
* @author	E.E. Javier Gaspar + U.G. Super-Team
* @version	1.0 - October 15th, 2004
*/


/* #include <conio.h>
 #include <dos.h> */
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "table.h"
#include "strin.h"
#include "timer.h"


#ifdef __cplusplus
  #define __CPPARGS ...
#else
  #define __CPPARGS 
#endif


static void change_IVT (unsigned int int_type, void interrupt (*new_isr)(), 
			void interrupt (*old_isr)() );


/** ~~~~~~~~~~~~~~~~ Direcciones de puertos y registros ~~~~~~~~~~~~~~~~~~~~ **/

#define IRQ4	4		/* UART interrupt number */
#define IRQ7	7		/* LPT  interrupt number */
#define UART	0x3F8		/* UART base port address (COM1) */
#define LPT1	0x378		/* LPT  base port address */
#define PIC1	0x20		/* PIC  base port address */

/* Registros del LPT1 */
#define DATA_P	(LPT1+0)	/* Data Register */
#define STATUS	(LPT1+1)	/* Status Register */
#define CONTROL	(LPT1+2)	/* Control Register */

/* Registros del UART */
#define DATA_S	(UART+0)	/* In this code: Rx buffer (rd only) */
#define IER	(UART+1)	/* Interrupt Enable Register (NOTE 1) */
#define IIR	(UART+2)	/* Interrupt Identification Register (NOTE 2) */
#define LCR	(UART+3)	/* Line Control Register (NOTE 3) */
#define LSR	(UART+5)	/* Line Status Register (NOTE 4) */

/* NOTE
 *
 *	1. _Para habilitar la recepción del UART hay que setear el bit 0 (LSB)
 *	   del IER
 *	   _Si se desea desactivar el modo de transmisión hay que resetear el
 *	   bit 1 del IER.
 *
 *	2. _Para indentificar a qué se debió la interrupción (Rx, Tx, etc)
 *	   Es para lectura exclusivamente (sino se accede a otro registro)
 *	   _Si se desactivó el modo transimisón se facilita el trabajo de
 *	   recepción, pues sólo se recibirán interrupciones por recepción, y
 *	   por ende no será necesario estudiar el IIR ante una interrupción
 *	   para averiguar si fue por recepción o por transmisión.
 *
 *	3. _Maneja el funcionamiento del puerto serie UART
 *	   _Si se escribe una palabra con el bit 7 (MSB) seteado se estará
 *	   accediendo a los "Divisor Latch Numbers" (DLN) que controlan el
 *	   baudeaje de Tx y Rx. El registro DATA_S se "transforma" en el DLN LSB
 *	   y el registro IER en el DLN MSB.
 *	   _Si se escribe una palabra con el bit 7 (MSB) en 0 se trabajará luego
 *	   con los registros de Tx/Rx y el IER en DATA_S e IER respectivamente
 *	   (las direcciones de puerto 0x3F8 y 0x3F9), y *no* con los DLN.
 *
 *	4. _El registro de estado del UART. El bit 0 (LSB) en alto voltaje
 *	   indica que el registro de recepción está lleno (para polling)
 *	   El bit 2 indica error de paridad (si fue habilitada en el LCR)
 */


/** ~~~~~~~~~~~~~~~ Variables y constantes globales ~~~~~~~~~~~~~~~~~~~~~~~~ **/

#define STRLEN  21
char text[STRLEN];	/* char * recibido por el UART */
int END_MSG = 0;	/* Bandera de fin de recepción (ver uartisr) */

string* win_string = NULL;	/* TAD que guarda el char * recibido */
char* str_to_print = NULL;	/* Lo que se manda a los display */

int base = 0, offset = 0, picaddr = 0, nxt_to_pr = 0;

timer *timer_to_print = NULL;	/* TAD que regula la velocidad de impresión */
#define DELAY   1000		/* Demora en la impresión */

void interrupt (*oldhandler4)(__CPPARGS);
void interrupt (*oldhandler7)(__CPPARGS);


/** ~~~~~~~~~~~~~~~ ISR: rutinas de manejo de interrupciones ~~~~~~~~~~~~~~~~ */

/* Manejador de interrupciones ante la recepción de un caracter en el UART 
 * Se asume que el mensaje termina cuando llega el caracter '\0'
 */
void interrupt uartisr (__CPPARGS)
{
	static unsigned int last_char = 0;
	
	disable (); /* Atomicidad ON */
	
	if (!END_MSG) {
		/* Guardamos el caracter que llegó */
		text[last_char] = (char) inp (DATA);
		
		/* Revisamos si era fin de mensaje */
		if (text[last_char] == '\0')
			END_MSG = 1;
		
		/* Nos movemos a la siguiente posición */
		last_char = (last_char + 1) % STRLEN;
	}
	
	outport(PIC1,0x20); /* EOI hacia al PIC */
	
	enable (); /* Atomicidad OFF */
}



void interrupt lptisr (__CPPARGS)
{
	int ans = 0;
	int ascii_code = 0;
	int display_code = 0;

	disable();

/* Cálculo de la información a mostrar por el display: */
	
	/* Obtenemos el código ascii del caracter a imprimir */
	ascii_code = str_to_print[base + offset];

	/* Traducimos ese caracter según la tablita */
	display_code = map_ascii[ascii_code];

	outport (DATA_P, display_code);		/* Output data */
	offset = (offset + 1) % DISPLAY_SIZE;	/* Update offset */
	
/* Selección del display por donde se muestra esa información: */
	
	ans = inp (CONTROL) & 0x10;	/* Modo "activar display indicado" */
	outport (CONTROL, ans | dir_order[offset]); /* Activación del display */
	
/* Actualización de la información a mostrar por el display: */
	
	inc_timer (timer_to_print);
	if (timeout_timer (timer_to_print)) {
		/* Reinicializamos la cuenta */
		start_timer (timer_to_print);

		nxt_to_pr = (nxt_to_pr+1) % strlen(text);
		str_to_print = string_slice_right (win_string, nxt_to_pr, DISPLAY_SIZE);
	}

	outport(picaddr,0x20);	/* PIC end of interrupt */

	enable();
}


/** ~~~~~~~~~~~~~~~~~~~~~~~~ Programa principal ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/

int main (int argc, char *argv[])
{
	int int_lpt = 0, int_uart = 0;
	int pic_mask_lpt = 0, pic_mask_uart = 0;
	int uart_ctrl = 0, uart_int = 0;

	int_lpt  = IRQ7 + 8;	/* int_lpt  == type 15 */
	int_uart = IRQ4 + 8;	/* int_uart == type 12 */
	picaddr  = PIC1;
	pic_mask_lpt  = 1;
	pic_mask_lpt  = pic_mask_lpt  << IRQ7; /* 1000 0000 == 0x80 */
	pic_mask_uart = 1;
	pic_mask_uart = pic_mask_uart << IRQ4; /* 0001 0000 == 0x10 */

	argc = argc; argv = argv; /* Para que no se queje el compilador */
	memset (text, '\0', STRLEN);
	
/* Manejo del puerto serie (uart) */
	
	/* Modo conservativo */
	uart_ctrl = inp (LCR);
	uart_int  = inp (IER);
	
	/* Deshabilitamos la interrupciones del uart, para que no molesten */
	outport (IER, 0x00);
	
	/* Preparamos el IVT */
	change_IVT ((unsigned int) int_uart, uartisr, oldhandler4);
	
	/* Escogemos baudeaje */
	outport (LCR, 0x80);		/* DLAB := 1 */
	outport (DATA_S, 0x18); 	/* DLN LSB */	/* Baudeaje escogido: */
	outport (IER, 0x00);		/* DLN MSB */	/*      4800 BPS      */
	
	/* Escogemos modo de trabajo */
	outport (LCR, 0x1F);	/* DLAB := 0 + Parity enabled + even parity +
				 * 2 stop bits + 8 bit word length */
	
	/* Desenmascaramos el PIC para que atienda los IRQ4 del uart */
	outport (picaddr+1, inp (picaddr+1)&(0xFF-pic_mask_uart));
	
	/* Habilitamos sólo las recepciones (notar que DLAB == 0) */
	outport (IER, 0x01);
	
	printf ("Receiving message through serial port\n");
	while (!END_MSG) ; /* Hasta recibir todo el mensaje por el uart
			    * no imprimiremos nada por el lpt
			    */
	/* Deshabilitamos las recepciones para que no nos cambien el mensaje */
	outport (IER, 0x00);
// 	DEBUGGING INFO
// 	printf ("Mensaje recibido por el puerto serie: %s\n", text);
	
/* Manejo del puerto paralelo (lpt) */
	
	/* Make sure port is in forward direction */
	outport (CONTROL, inp (CONTROL)&0xDF);
	outport (DATA,0xFF);
	
	/* Preparamos el IVT */
	change_IVT ((unsigned int) int_lpt, lptisr, oldhandler7);
	
	/* Desenmascaramos el PIC para que atienda los IRQ7 del lpt */
	outport (picaddr+1, inp (picaddr+1)&(0xFF-pic_mask_uart));
	
	/* Habilitamos las transmisiones del lpt */
	outport (CONTROL, inp(CONTROL)|0x10);
	
/* Preparación de los TADs para impresión por el display */
	
	/* Encapsulamos el mensaje en el TAD String */
	win_string = string_create (text);
	str_to_print = string_get_front (win_string, 0, DISPLAY_SIZE);
	free (str_to_print);

	/* Generamos el timer para velocidad de impresión (ver lptisr) */
	timer_to_print = setup_timer (DELAY);
	start_timer (timer_to_print);
	
/* Main program */
	
	printf ("Interrupt is enabled. Main program prints some values.\n");
	for (;!kbhit();) {
		printf("base = %i\toffset = %i\tvalue[b+o] = 0x%02X\t",
			base,offset,map_ascii[base+offset]);
		printf("STATUS = 0x%X\r",inp(STATUS)&0x40);
	}
	
/* Desinstalación de los puertos (paralelo y serie) */
	
	/* Deshabilitamos las transmisiones del lpt */
	outport (CONTROL, inp (CONTROL)&0xEF);
	
	/* Re-enmascaramos el PIC para que ya no atienda los IRQ7 */
	outport (picaddr+1, inp (picaddr+1)|pic_mask_lpt);
	
	/* Sacamos datos nulos por el lpt */
	outport (DATA, 0x00);
	
	/* Restablecemos el uart a su estado original */
	outport (IER, uart_int);
	outport (LCR, uart_ctrl);
	
	/* Re-enmascaramos el PIC para que ya no atienda los IRQ4 */
	outport (picaddr+1, inp (picaddr+1)|pic_mask_uart);
	
	/* Volvemos el IVT a su estado original */
	change_IVT ((unsigned int) int_lpt, oldhandler7, lptisr);
	change_IVT ((unsigned int) int_lpt, oldhandler4, uartisr);
	
	/* Liberamos todos los recursos */
	win_string = string_destroy (win_string);
	free (str_to_print);
	timer_to_print = stop_timer (timer_to_print);

	/* Quit program */
	printf ("\nExit interrupt successfully. Quit program\n");

	return 0;
}


/** ~~~~~~~~~~~~~~~~~~~~~~~ Subrutinas de ayuda ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/

/* Administrador del vector de interrupciones (IVT)
 * Setea el manejador de interrupciones new_isr en la posición int_type.
 * El manejador que estaba previamente en IVT[int_type] es guardado en old_isr
 */
static void change_IVT (unsigned int int_type, void interrupt (*new_isr)(), 
			void interrupt (*old_isr)() )
{
	ASSERT (new_isr != NULL);
	ASSERT (old_isr != NULL);
	
	old_isr = getvect (int_type); /* Guardamos el viejo ISR */
	disable ();
	setvect (int_type, new_isr); /* Instalamos el nuevo ISR atómicamente */
	enable ();
	
	return;
}
