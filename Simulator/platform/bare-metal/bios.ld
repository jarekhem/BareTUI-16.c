ENTRY(_reset)

_Segment_Size = 64K;
_ROM_Size = 32K;
_ROM_Reserved = 4;
_RAM_Reserved = 0x500;

MEMORY {
    /* 64KB ROM image mapped at CS=0xF000 */
    ROM (rx) : ORIGIN = _Segment_Size - _ROM_Size, LENGTH = 32K
    /* VMA for RAM must also start at 0x0000 so offsets fit in 16 bits.
     * The physical location is handled by DS=0x1000 in boot.s
     */
    RAM (rw) : ORIGIN = 0x0000 + _RAM_Reserved, LENGTH = _Segment_Size - _RAM_Reserved
}

SECTIONS {
    .text : {
        *(.text.init)
        *(.text)
        *(.text.*)
    } > ROM

    .data : {
        __data_start = .;
        *(.data)
        *(.data.*)

        *(.rodata)
        *(.rodata.*)

        __data_end = .;
    } > RAM AT > ROM

    __rom_data_start = LOADADDR(.data);

    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss)
        *(COMMON)
        __bss_end = .;
    } > RAM

    /* Magic Reset Vector at offset 0xFFF0 */
    .reset 0xFFF0 : {
        KEEP(*(.reset_vector))
    } > ROM

    .reserved (_Segment_Size - _ROM_Reserved):
    {
        KEEP(*(.reserved_pad))
    } >  ROM

}