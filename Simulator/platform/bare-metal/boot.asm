BITS 16
CPU 8086

; =====================================================================
; RESET VECTOR (Mapped to physical 0xFFFF0)
; =====================================================================
SECTION .reset_vector progbits alloc exec
global _reset
_reset:
    jmp 0xF000:start16

SECTION .reserved_pad progbits alloc
    dw 0xFFFF
    dw 0xFFFF

; =====================================================================
; INITIALIZATION CODE (Mapped to physical 0xF0000)
; =====================================================================
SECTION .text.init progbits alloc exec
global start16
extern app_main

; Linker script symbols
extern __data_start
extern __data_end
extern __rom_data_start
extern __bss_start
extern __bss_end

global systemInit_done
extern systemInit_start
extern systemInit_error

extern setupVectors

start16:
    cli
    cld
    jmp systemInit_start

systemInit_done:
    ; 1. Setup Stack and Data Segments in RAM (Segment 0x0000)
    xor ax, ax
    mov ss, ax
    mov sp, 0xFFFE       
    mov ds, ax           
    mov es, ax

    call setupVectors

    ; 2. Execute VGA Option ROM (0xC000:0003)
    call 0xC000:0x0003

    ; 3. Restore Data Segments (VGA ROM might have changed them)
    xor ax, ax
    mov ds, ax           
    mov es, ax

    ; 4. Copy initialized data (.data) from ROM to RAM
    mov cx, __data_end
    sub cx, __data_start
    jz .skip_data
    mov si, __rom_data_start
    mov di, __data_start
.copy_data:
    cs lodsb    ; NASM way of adding segment override
    stosb
    loop .copy_data
.skip_data:

    ; 5. Clear BSS section in RAM
    mov cx, __bss_end
    sub cx, __bss_start
    jz .skip_bss
    mov di, __bss_start
    xor ax, ax
    rep stosb
.skip_bss:
    sti
    ; 6. Call the C engine
    call app_main

    ; 7. Infinite Halt Loop
.halt:
    hlt
    jmp .halt