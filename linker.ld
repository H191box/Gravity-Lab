/* Gravity Lab — Linker Script
 *
 * Memory layout for GBA:
 *   ROM:   0x08000000 (up to 32MB cartridge ROM)
 *   EWRAM: 0x02000000 (256KB external work RAM)
 *   SRAM:  0x0E000000 (64KB save RAM)
 *
 * The .rom_header section (GBA header + ARM stub) goes first in ROM.
 * Code (.text) and read-only data (.rodata) follow in ROM.
 * Initialized data (.data) is stored in ROM (LMA) but copied to EWRAM (VMA) at startup.
 * Uninitialized data (.bss) is zeroed in EWRAM at startup.
 */

OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
    ROM   (rx)  : ORIGIN = 0x08000000, LENGTH = 32M
    EWRAM (rwx) : ORIGIN = 0x02000000, LENGTH = 256K
    SRAM  (rw)  : ORIGIN = 0x0E000000, LENGTH = 64K
}

SECTIONS
{
    /* --- ROM Section --- */
    . = ORIGIN(ROM);

    /* GBA ROM header + ARM entry stub (must be first) */
    .rom_header : {
        *(.rom_header)
    } > ROM

    /* Code — all executable sections */
    .text : {
        *(.text.start)        /* _start entry point first */
        *(.text .text.*)
    } > ROM

    /* Read-only data (constants, strings, tables) */
    .rodata : {
        *(.rodata .rodata.*)
    } > ROM

    /* --- EWRAM Sections --- */
    . = ORIGIN(EWRAM);

    /* Initialized global/static data
     * Stored in ROM as init values (AT> ROM = load address in ROM)
     * Copied to EWRAM at startup by ARM entry stub */
    .data : {
        __data_start = .;
        *(.data .data.*)
        __data_end = .;
    } > EWRAM AT> ROM

    /* Uninitialized data (zeroed at startup) */
    __bss_start = .;
    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    } > EWRAM
    __bss_end = .;

    /* Symbol for ROM-to-EWRAM .data copy */
    __data_rom_start = LOADADDR(.data);

    /* --- SRAM Section (for save data) --- */
    .sram (NOLOAD) : {
        *(.sram .sram.*)
    } > SRAM

    /* Discard unwanted sections */
    /DISCARD/ : {
        *(.ARM.attributes)
        *(.comment)
        *(.note.*)
    }
}
