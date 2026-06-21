/* ============================================================
 *  Gravity Lab — Main Entry Point
 *  Circuit Racing Puzzle Game for Game Boy Advance
 *
 *  Build: devkitPro + gba.specs
 *  gba.specs provides: ROM header, ARM→Thumb stub, .data copy,
 *  .bss zeroing, stack setup. Entry point is main().
 *
 *  We use our own gba_header.h for register definitions (not gba.h)
 *  to avoid macro conflicts, and handle VBlank manually.
 * ============================================================ */

#include "gravity_lab.h"

int main(void) {
    /* Initialize all game subsystems
     * sys_init() handles display, VBlank ISR, VRAM, palette, OAM */
    game_init();

    /* -------------------------------------------------------
     *  Main Game Loop — 60fps on real GBA hardware.
     * ------------------------------------------------------- */
    while (1) {
        /* Step 1: Poll keypad input */
        input_update();

        /* Step 2: Update game logic */
        game_update();

        /* Step 3: Render everything */
        game_render();

        /* Step 4: Synchronize to VBlank */
        sys_wait_vblank();
    }
}
