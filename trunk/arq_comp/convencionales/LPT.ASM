;/**
;*      Centronic interrupt program example 1 (in Assembler)
;*
;*
;* @author  E.E. Javier Gaspar
;* @version 1.0 - September 30th, 2006
;*/

;   /--------------------------------\
;   |         DEFINE CONSTANTS       |
;   \--------------------------------/
;-----  Key characters data	-----
cr		EQU 13		; Define carriage return.
lf		EQU 10		; Define line feed.
esc		EQU 27		; define escape.
;-----  Interrupt vector data	-----
IRQ		EQU 7		; Interrupt number LPT==IRQ7.
IRQnro		EQU (IRQ+8)	; Interrupt vector position. (IRQ7 == Type 15)
IRQvec		EQU (4*IRQnro)	; Interrupt vector base address (table).
;-----  Miscellaneous data	-----
alfa		EQU 48		; Alphanumeric base	[48	65	97]
wrap		EQU 10		; Alphanumeric modulo	[10	26	26]
max		EQU 90		; Counter
;-----  Port addresses data	-----
lpt1addr	EQU 0378h	; LPT1 base address (03BCh in PC-XT).
picaddr		EQU 20h		; pic base address.
uart1addr	EQU 03F8h	; uart1 base address (02F8h in PC-XT).
;-----  Port masks data		-----
lptmask		EQU 10h		; LPT1 interrupt mask enable/disable.

;   /--------------------------------\
;   |         CODE SEGMENT           |
;   \--------------------------------/
cseg    SEGMENT
cseg    ENDS

;   /--------------------------------\
;   |         DATA SEGMENT           |
;   \--------------------------------/
dseg    SEGMENT
cntr	DW	max
base	DW	alfa
index	DW	0
picmask DB	0			; PIC interrupt mask enable/disable.
ascii   DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h  ;   0 -  15 [00 - 0f]
	DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h  ;  16 -  31 [10 - 1f]
	DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h
	DB	00h, 00h, 00h, 00h, 00h, 00h, 00h, 00h  ;  32 -  47 [20 - 2f]
	DB	3fh, 06h, 5bh, 4fh, 66h, 6dh, 7ch, 07h
	DB	7fh, 67h, 00h, 00h, 00h, 00h, 00h, 00h  ;  48 -  63 [30 - 3f]
	DB	00h, 77h, 7fh, 39h, 5eh, 79h, 71h, 3dh
	DB	76h, 30h, 0eh, 00h, 38h, 37h, 54h, 3fh  ;  64 -  79 [40 - 4f]
	DB	73h, 67h, 50h, 6dh, 78h, 3eh, 00h, 00h
	DB	00h, 6eh, 00h, 00h, 00h, 00h, 00h, 00h  ;  80 -  95 [50 - 5f]
	DB	00h, 5fh, 7ch, 58h, 5eh, 7bh, 71h, 6fh
	DB	74h, 06h, 1eh, 00h, 38h, 37h, 54h, 5ch  ;  96 - 111 [60 - 6f]
	DB	73h, 67h, 50h, 6dh, 78h, 1ch, 00h, 00h
	DB	00h, 6eh, 00h, 00h, 00h, 00h, 00h, 00h  ; 114 - 127 [70 - 7f]
text    DB	"Type any letter, number or punctuation key. "
	DB	"Esc to end program.", cr, lf, lf, '$'
data	DB	1
IntVectSave	DD	0
dseg    ENDS

;   ·--------------------------------·
;   |         STACK SEGMENT          |
;   ·--------------------------------·
sseg    SEGMENT
stack	DB  256 DUP(0)          ; define stack size
sseg    ENDS

;   /--------------------------------\
;   | INTERRUPT VECTOR TABLE SEGMENT |
;   \--------------------------------/
iseg    SEGMENT AT 0
	ORG IRQvec
IRQaddr LABEL DWORD
iseg    ENDS

;   /--------------------------------\
;   |         CODE SEGMENT           |
;   \--------------------------------/
cseg    SEGMENT
	ASSUME  cs:cseg, ds:dseg, es:iseg, ss:sseg
main    PROC    FAR
	call init_seg_reg	    ; Guardamos a mano el data seg y el extra seg
	call save_old_vec
	call set_lpt_vec

	mov cl, IRQ	            ; Set picmask var in accordance of the interrupt used.
	mov ah, 1
	shl ah, cl
	not ah
	mov [picmask], ah

	call set_irq_pic
	call enable_lpt_int

;_wait:
;    hlt
;    cmp cntr, 0
;    jnz _wait

	call cls                ; clear screen
	lea dx, text            ; point DX to text
	call msg

_loop1:
	mov ah, 01h
	int 16h
	jz  _loop1

	mov ah, 00h
	int 16h

	cmp al, esc
	jz  _end

	mov ah, 0Eh             ; BIOS: video service 0eh
	int 10h                 ; display character
	jmp _loop1

_end:
	call disable_lpt_int
	call reset_irq_pic
	call restore_old_vec

	mov dx, lpt1addr        ; turn display off.
	mov al, 00h
	out dx, al

	mov ax, 4C00h           ; terminate program and return to DOS.
	int 21h
main    ENDP

;   /--------------------------------\
;   |         SUBROUTINES            |
;   \--------------------------------/
	;   SET SEGMENT REGISTERS
init_seg_reg:
	mov ax, dseg            ; get address of data segment.
	mov ds, ax              ; store in ds register.
	mov ax, iseg            ; get vector interrupt table segment.
	mov es, ax              ; store in es register.
	ret

	;   SAVE OLD VECTOR
save_old_vec:
	mov ax, WORD PTR [IRQaddr]      ; save the current vector entry in the
	mov WORD PTR [IntVectSave], ax  ; double word variable "IntVectSave"
	mov ax, WORD PTR [IRQaddr+2]
	mov WORD PTR [IntVectSave+2], ax
	ret

	;   SET NEW LPT VECTOR
set_lpt_vec:
	cli
	mov ax, OFFSET lptISR
	mov WORD PTR [IRQaddr], ax   ; patch the interrupt vector table with
	mov ax, SEG lptISR           ; the address of our ISR
	mov WORD PTR [IRQaddr+2], ax
	sti
	ret

	;   SET PIC IRQ (LPT)
set_irq_pic:
	in  al, [picaddr+1]     ; read existing bits (PIC interrupt mask register).
	and al, picmask         ; turn on IRQ7 (LPT), negative logic.
	out [picaddr+1], al     ; write result back to PIC.
	ret

	;   ENABLE LPT INTERRUPT
enable_lpt_int:
	mov dx, lpt1addr+2
	in  al, dx
	or  al, lptmask         ; set Control Register Interrupt bit (4th), positive
	out dx, al              ; logic and enable LPT interrupt.
	ret

	;   DISABLE LPT INTERRUPT
disable_lpt_int:
	mov dx, lpt1addr+2	; disable LPT interrupt.
	in  al, dx
	mov ah, lptmask
	not ah
	and al, ah
	out dx, al
	ret

	;   RESET PIC IRQ (LPT)
reset_irq_pic:
	in  al, [picaddr+1]     ; mask PIC.
	mov ah, [picmask]
	not ah
	or  al, ah
	out [picaddr+1], al
	ret

	;   RESTORE OLD VECTOR
restore_old_vec:
	cli
	mov ax, WORD PTR [IntVectSave]    ; restore de old interrupt vector.
	mov WORD PTR [IRQaddr], ax
	mov ax, WORD PTR [IntVectSave+2]
	mov WORD PTR [IRQaddr+2], ax
	sti
	ret

	;   CLEAR SCREEN
cls:
	mov ax, 0600h
	mov bh, 07h
	mov cx, 0000h
	mov dx, 184Fh
	int 10h
	ret

	;   PRINT MSG (pointed by dx)
msg:
	mov ah, 09h             ; DOS: function 09h
	int 21h
	ret

;   /--------------------------------\
;   |   INTERRUPT SERVICE ROUTINE    |
;   \--------------------------------/
lptISR  PROC    FAR
	cli

	; call ascii_to_disp
	; call decode_pos
	; dec  [cntr]             ; decrement counter memory.
	; call end_of_int

	call test_led
	call end_of_int	

	sti
	iret                    ; return from interrupt.
lptISR  ENDP

	;   CONVERT ASCII TO DISPLAY
	;   SEND CHAR TO LPT DATA PORT
ascii_to_disp:
	push ax                 ; save registers on the stack.
	push dx
	push bp
	push si

	mov bp, [base]          ; get ascii data.
	mov si, [index]
	mov al, ascii[bp+si]    ; ascii[bp][si]

	mov dx, lpt1addr	      ; data to display.
	out dx, al

	pop si                  ; restore registers from the stack.
	pop bp
	pop dx
	pop ax
	ret

	;   DECODE DISPLAY POSITION
	;   SEND DATA TO LPT CONTROL PORT
decode_pos:
	push ax                 ; save registers on the stack.
	push dx
	push si

	mov ax, [index]         ; index wrap
	inc ax
	mov dl, wrap
	idiv dl
	mov al, ah
	mov ah, 00h
	mov [index], ax

	pop si                  ; restore registers from the stack.
	pop dx
	pop ax
	ret

	;   TEST LED ONTO LPT DATA PORT
test_led:
	push ax                 ; save registers on the stack.
	push dx

	mov al, [data]
	mov dx, lpt1addr
	out dx, al

	cmp al, 0
	jz  reset
	sal [data], 1
	jmp endif
reset:
	mov [data], 1
endif:
	pop dx                  ; restore registers from the stack.
	pop ax
	ret

	;   SEND END OF INTERRUPT TO PIC
end_of_int:
	push ax
	mov al, picaddr         ; end of interrupt byte.
	out [picaddr], al       ; EOI to PIC command reg
	pop ax
	ret

cseg    ENDS
	END

