#ifndef TILES_H
#define TILES_H

#include "gba_types.h"

/* Number of unique tile graphics */
#define NUM_TILES 19

/* Tile graphics are generated programmatically.
 * Each tile is 8×8 pixels, 4bpp = 32 bytes = 16 halfwords.
 * Tiles are loaded into BG CharBase 0 (0x06000000).
 *
 * Tile layout:
 *   0: Empty/void (solid dark blue-black)
 *   1: Floor (metallic gray grid)
 *   2: Floor with left edge line
 *   3: Floor with right edge line
 *   4: Floor with top edge line
 *   5: Floor with bottom edge line
 *   6: Wall (solid dark metal)
 *   7: Wall top (highlight edge)
 *   8: Wall corner TL
 *   9: Wall corner TR
 *  10: Wall corner BL
 *  11: Wall corner BR
 *  12: Checkpoint gate (green striped)
 *  13: Boost pad (yellow arrows)
 *  14: Finish line (checkerboard)
 *  15: Obstacle spawn (red X)
 *  16: Deco light (bright floor accent)
 *  17: Deco dark (dark floor accent)
 *  18: Start position marker
 */

/* -------------------------------------------------------
 *  tiles_load — Generate and load all tile graphics into VRAM
 *  Tiles are generated at runtime (no external binary assets).
 *  Loads into BG CharBase 0.
 * ------------------------------------------------------- */
void tiles_load(void);

#endif /* TILES_H */
