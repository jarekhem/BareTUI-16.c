CPU 8086
BITS 16

%include "cpu_regs.inc"
;%include "ramTest.inc"

extern systemInit_done

global systemInit_start
global systemInit_error

biosCodeSeg	equ 0xF000
ramTestBlockSize_kB     equ 16

pic_freq	equ	1193182	; PIC input frequency - 14318180 MHz / 12
pit_ch0_reg	equ	40h
pit_ch1_reg	equ	41h
pit_ch2_reg	equ	42h
pit_ctl_reg	equ	43h
ppi_pb_reg	equ	61h	; 8255 PPI port B I/O register


SECTION .text

systemInit:
    dw cpu_init
    dw timer8254_init
    ; dw dma8237_init
;    dw testLowRam
    dw lowRamTested
    dw systemInit_done

systemInit_error:
	mov	al,0B6h
	out	pit_ctl_reg,al		; PIT - channel 2 mode 3
	mov	ax,pic_freq/400		; 400 Hz signal
	out	pit_ch2_reg,al
	mov	al,ah
	out	pit_ch2_reg,al
	in	al,ppi_pb_reg
	or	al,3			; turn speaker on and enable
	out	ppi_pb_reg,al		; PIT channel 2 to speaker
.1:
    hlt
    jmp .1

systemInit_start:
    mov ax, biosCodeSeg
    mov ds, ax
    mov ss, ax
    mov sp, systemInit
    ret

cpu_init:
    mov al, 0x00 ;0x08
    mov dx, SBCR
    out dx, al

    mov al, 0x01
    mov dx, SCTL
    out dx, al

    mov al, 0x00
    mov dx, OPHA
    out dx, al

    mov al, 0x40
    mov dx, TULA
    out dx, al

    mov al, 0x04
    mov dx, OPSEL
    out dx, al

    mov al, 0x1C
    mov dx, TCKS
    out dx, al
    
    ; xor al, al
    ; mov dx, EXWB
    ; out dx, al

    ; mov al, 0xF0
    ; mov dx, WCY1
    ; out dx, al
    ret

timer8254_init:
    mov     al,01010100b                    ; ic 8253 inits memory refresh
    out     43h,al                          ;  ...chan 1 pulses ic 8237 to
    mov     al,12h                          ;  ...dma every 12h clock ticks
    out     41h,al                          ;  ...64K done in 1 millisecond
    mov     al,01000000b                    ; Latch value 12h in 8253 clock
    out     43h,al                          ;  ...chip channel 1 counter
    ret

;testLowRam:
;    xor ax, ax
;    jmp ramTestArea
;
lowRamTested:
;    jc systemInit_error                       ; low ram invalid - halt!
    xor ax, ax      ; fill with zeros
    mov cx, ramTestBlockSize_kB * 512
    xor di, di
    rep stosw
    ret