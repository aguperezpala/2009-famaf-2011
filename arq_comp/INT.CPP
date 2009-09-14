/* Parallel Port Interrupt Polarity Tester */
/* 2nd February 1998 */
/* Copyright 1997 Craig Peacock */
/* WWW - http://www.senet.com.au/~cpeacock */
/* Email - cpeacock@senet.com.au */
#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#define PORTADDRESS 0x378 /* Enter Your Port Address Here */
#define IRQ 7 /* IRQ Here */
#define DATA PORTADDRESS+0
#define STATUS PORTADDRESS+1
#define CONTROL PORTADDRESS+2
#define PIC1 0x20
#define PIC2 0xA0
int interflag; /* Interrupt Flag */
int picaddr; /* Programmable Interrupt Controller (PIC) Base Address */



void interrupt (*oldhandler)(...);
void interrupt parisr(...) /* Interrupt Service Routine (ISR) */
{
interflag = 1;
outportb(picaddr,0x20); /* End of Interrupt (EOI) */
}
void main(void)
{
int c;
int intno; /* Interrupt Vector Number */
int picmask; /* PIC's Mask */
/* Calculate Interrupt Vector, PIC Addr & Mask. */

intno = IRQ + 0x08;
picaddr = PIC1;
picmask = 1;
picmask = picmask << IRQ;


outportb(CONTROL, inportb(CONTROL) & 0xDF); /* Make sure port is in Forward Direction */
outportb(DATA,0xFF);
oldhandler = getvect(intno); /* Save Old Interrupt Vector */
setvect(intno, parisr); /* Set New Interrupt Vector Entry */
outportb(picaddr+1,inportb(picaddr+1) & (0xFF - picmask)); /* Un-Mask Pic */
outportb(CONTROL, inportb(CONTROL) | 0x10); /* Enable Parallel Port IRQ's */
clrscr();

printf("Parallel Port Interrupt Polarity Tester\n");
printf("IRQ %d : INTNO %02X : PIC Addr 0x%X : Mask 0x%02X\n",IRQ,intno,picaddr,picmask);
interflag = 0; /* Reset Interrupt Flag */
delay(10);

printf("esperando inter...");
while (interflag == 0);
	printf("Llego");

getch();



outportb(CONTROL, inportb(CONTROL) & 0xEF); /* Disable Parallel Port IRQ's */
outportb(picaddr+1,inportb(picaddr+1) | picmask); /* Mask Pic */
setvect(intno, oldhandler); /* Restore old Interrupt Vector Before Exit */
}