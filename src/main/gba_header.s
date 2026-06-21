@ GBA ROM Header + ARM entry stub
@ ============================================================
@ GBA BIOS enters at 0x08000000 in ARM mode.
@ This file contains:
@   1. GBA ROM Header (192 bytes = 0xC0) with ARM branch
@   2. ARM entry stub: set stack, copy .data, zero .bss, switch to Thumb
@
@ The linker places this in .rom_header at the start of ROM.
@ After the stub switches to Thumb, it jumps to _start (in main.c).
@ ============================================================

    .arm
    .section .rom_header

@ ============================================================
@  GBA ROM Header
@  GBA standard header layout:
@    0x000: ARM branch (entry point)
@    0x004: Nintendo logo (156 bytes)
@    0x0A4: Game title (12 bytes)
@    0x0B0: Game code (4 bytes)
@    0x0B4: Maker code (4 bytes)
@    0x0B8: Fixed value, unit code, device type, reserved, version, checksum, reserved
@ ============================================================

arm_start:
    b   arm_entry          @ 0x000: Branch to entry stub (forward ref)

    .space 156              @ 0x004: Logo placeholder (156 bytes)

    .ascii "GRAVITY LAB"    @ 0x0A4: Title (12 bytes)
    .ascii "GL01"           @ 0x0B0: Game code (4 bytes)
    .ascii "Z00"            @ 0x0B4: Maker code (4 bytes)
    .byte 0x96              @ 0x0B8: Fixed value
    .byte 0x00              @ 0x0B9: Main unit code
    .byte 0x00              @ 0x0BA: Device type
    .byte 0x07              @ 0x0BB: Reserved
    .byte 0x00              @ 0x0BC: Software version
    .byte 0x00              @ 0x0BD: Complement check
    .word 0x00000000        @ 0x0BE: Reserved

    @ Pad to 192 bytes (0xC0)
    @ Current: 4 + 156 + 12 + 4 + 4 + 1 + 1 + 1 + 1 + 1 + 1 + 4 = 190
    .space 2

@ ============================================================
@  ARM Entry Stub
@  Sets up runtime environment, then switches to Thumb.
@  arm_entry is at offset 0xC0 from ROM base.
@ ============================================================
    .global arm_entry
arm_entry:
    @ Step 1: Set up stack pointer — top of EWRAM (256KB)
    ldr sp, =0x02040000

    @ Step 2: Copy .data initial values from ROM to EWRAM
    ldr r0, =__data_rom_start
    ldr r1, =__data_start
    ldr r2, =__data_end
data_copy_loop:
    cmp r1, r2
    bge data_copy_done
    ldr r3, [r0], #4
    str r3, [r1], #4
    b   data_copy_loop
data_copy_done:

    @ Step 3: Zero BSS section in EWRAM
    ldr r0, =__bss_start
    ldr r1, =__bss_end
    mov r2, #0
bss_zero_loop:
    cmp r0, r1
    bge bss_zero_done
    str r2, [r0], #4
    b   bss_zero_loop
bss_zero_done:

    @ Step 4: Switch to Thumb mode and jump to C _start
    @ _start is defined in main.c with __attribute__((section(".text.start")))
    ldr r0, =_start
    add r0, r0, #1          @ Set Thumb bit (bit 0 = 1)
    bx  r0                     @ Branch and exchange to Thumb mode
