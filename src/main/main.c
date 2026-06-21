/* ============================================================
 *  Gravity Lab — Main Entry Point
 *  Circuit Racing Puzzle Game for Game Boy Advance
 *
 *  This file contains the ROM entry point and the main
 *  game loop. The GBA BIOS jumps to address 0x08000000
 *  on boot, which is where our code begins.
 * ============================================================ */

#include "gravity_lab.h"

/* -------------------------------------------------------
 *  ROM Header Entry Point
 *
 *  The GBA expects the first 4 bytes of ROM to be a
 *  branch instruction to the actual entry point.
 *  The linker script places this at 0x08000000.
 *
 *  The Makefile generates a .rom_header section with
 *  the initial entry point. Our _start function is
 *  placed right after the header.
 * ------------------------------------------------------- */

/* Entry point — called from ROM header */
void _start(void) __attribute__((section(".text.start")));

void _start(void) {
    /* Initialize all game subsystems */
    game_init();

    /* -------------------------------------------------------
     *  Main Game Loop
     *
     *  1. Read input
     *  2. Update game logic
     *  3. Render to screen
     *  4. Wait for VBlank (swap buffers)
     *
     *  This runs at 60fps on real GBA hardware.
     * ------------------------------------------------------- */
    while (1) {
        /* Step 1: Poll keypad input */
        input_update();

        /* Step 2: Update game state */
        game_update();

        /* Step 3: Render everything */
        game_render();

        /* Step 4: Synchronize to VBlank */
        sys_wait_vblank();
    }
}

/* -------------------------------------------------------
 *  Interrupt Service Routines
 *
 *  These are referenced by the interrupt table.
 *  VBlank ISR is defined in system.c and installed at
 *  address 0x03000008 by sys_install_vblank_handler().
 * ------------------------------------------------------- */

/* Dummy handler to fill unused interrupt vector entries */
void dummy_irq_handler(void) __attribute__((interrupt("IRQ")));
void dummy_irq_handler(void) {
    /* Acknowledge all pending interrupts */
    REG_IF = REG_IF;
}
