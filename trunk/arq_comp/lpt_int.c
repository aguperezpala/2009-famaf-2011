/**
*	Centronic interrupt program example 1 (in C).
*
*
* @author	E.E. Javier Gaspar
* @version	1.0 - October 15th, 2004
*/

#define IRQn	7		/* LPT interrupt number */
#define LPT1    0x378		/* LPT base port address (3BC) */
#define PIC1    0x20		/* PIC base port address */

#define DATA	(LPT1+0)	/* Data Register */
#define STATUS	(LPT1+1)	/* Status Register */
#define CONTROL	(LPT1+2)	/* Control Register */

#include <conio.h>
#include <dos.h>
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


#define TEXT	"rodrigo puto "
#define MODULO	strlen(TEXT)
#define DELAY   100


char* str_to_print = NULL;
string* win_string = NULL;

int base = 0, offset = 0, wrap = DISPLAY_SIZE, picaddr = 0, nxt_to_pr = 0;
/* Cambié 8 en wrap por DISPLAY_SIZE */

timer *timer_to_print = NULL;

void interrupt (*oldhandler)(__CPPARGS);


void interrupt lptisr (__CPPARGS) {
/* En DATA escribimos el dígito a imprimir
 * Con CONTROL seleccionamos el display a imprimir
 * CONTROL: (MSB) - - - - ~17  16 ~14 ~1 (LSB)
 * Usamos la lineas: Strobe:Bit0, LineFeed:Bit1, Select_Printer:Bit3 ?
 * Son activas por bajo! (salvo bit2)
 */

	int ans = 0;
	int ascii_code = 0;
	int display_code = 0;


	disable();

	/* Obtenemos el código ascci del caracter a imprimir */
	ascii_code = str_to_print[base + offset];

	/* Traducimos ese caracter según la tablita */
	display_code = map_ascii[ascii_code];

	outport (DATA, display_code);    /* Output data */
	offset = (offset + 1) % wrap;		    /* Update offset */

	ans = inport (CONTROL) & 0x10;
	/* Porqué está Máscara?, Usamos los Bit0,1,3? */

	outport (CONTROL, ans | dir_order[offset]); /* See port02.c */

	/* Vemos si ya hay que mostrar otra cosa m�s all� */
	inc_timer (timer_to_print);

	if (timeout_timer (timer_to_print)) {

		/* Reinicializamos la cuenta */
		start_timer (timer_to_print);

		nxt_to_pr = (nxt_to_pr+1) % MODULO;
		str_to_print = string_slice_right (win_string, nxt_to_pr, DISPLAY_SIZE);
	}

	outport(picaddr,0x20);			    /* PIC end of interrupt */

	enable();
}


int main(int argc,char* argv[]) {
	int intno = 0, picmask = 0,i=0;


	intno = IRQn + 8;
	picaddr = PIC1;
	picmask = 1;
	picmask = picmask << IRQn; /* 0..0 0..0 0..0 010000000 */

	/* Make sure port is in forward direction */
	outport (CONTROL, inp (CONTROL)&0xDF);
	outport (DATA,0xFF);

	/* Save old interrupt vector */
	oldhandler = getvect (intno);

	/* Set new interrupt vector entry */
	disable ();
	setvect (intno,lptisr);
	enable ();

	/* Un-Mask Pic */
	outport (picaddr+1, inp (picaddr+1)&(0xFF-picmask));

	/* Enable parallel port IRQ's */
	outport (CONTROL, inp(CONTROL)|0x10);


	/* --- Setup the string to print --- */

	win_string = string_create (TEXT);
	ASSERT (win_string != NULL);

	/* Generamos el TAD String con su ventanita */
	str_to_print = string_get_front (win_string, 0, DISPLAY_SIZE);
	free (str_to_print);

	/* Generamos el timer que va a manejar el interrupt_handler */
	timer_to_print = setup_timer (DELAY);
	start_timer (timer_to_print);

	/*
	for ( i = 0 ; i < 20 ; i++) {

		str_to_print = string_slice_right (win_string, i%MODULO, DISPLAY_SIZE);

		printf ("str_to_print: %s\n", str_to_print);

		free (str_to_print);

		ASSERT (str_to_print != NULL);
	}
	*/

	/* ---                           --- */



	/* Main program */
	printf ("Interrupt is enabled. Main program prints some values.\n");
	for (;!kbhit();) {
	  printf("base = %i\toffset = %i\tvalue[b+o] = 0x%02X\t",
		 base,offset,map_ascii[base+offset]);
	  printf("STATUS = 0x%X\r",inp(STATUS)&0x40);

	}

	/* Disable parallel port IRQ's */
	outport (CONTROL, inp (CONTROL)&0xEF);

	/* Mask Pic */
	outport (picaddr+1, inp (picaddr+1)|picmask);

	/* Restore old interrupt vector before exit */
	disable ();
	setvect (intno, oldhandler);
	enable ();

	/* Output null data */
	outport (DATA, 0x00);

	/* Free all resources */
	win_string = string_destroy (win_string);
	free (str_to_print);

	/* Quit program */
	printf ("\nExit interrupt successfully. Quit program\n");

	return 0;
}


