#ifndef SPRITES_H
#define SPRITES_H

#include "gba_types.h"

/* GameSprite tile layout in OBJ VRAM:
 *   Tiles 0-3:    Ship frame 0 (16x16, normal)
 *   Tiles 4-7:    Ship frame 1 (16x16, tilt left)
 *   Tiles 8-11:   Ship frame 2 (16x16, tilt right)
 *   Tiles 12-15:  Ship frame 3 (16x16, thrust)
 *   Tiles 16-19:  Explosion fireball (16x16)
 *   Tiles 20-23:  Obstacle bar (16x16)
 *   Tiles 24-27:  Gold medal (16x16)
 *   Tiles 28-31:  Silver medal (16x16)
 *   Tiles 32-35:  Bronze medal (16x16)
 *   Tile 36:      8x8 particle (spark/glow)
 *   Tile 37:      8x8 particle (smoke)
 *   Tile 38:      8x8 particle (checkpoint marker)
 *
 * 4bpp sprite: each tile = 32 bytes = 16 halfwords
 * OBJ VRAM base: 0x06010000 (after BG VRAM)
 */

/* -------------------------------------------------------
 *  sprites_load — Generate and load all sprite graphics
 *  into OBJ VRAM. Sprites are generated at runtime
 *  using pixel primitives.
 * ------------------------------------------------------- */
void sprites_load(void);

#endif /* SPRITES_H */
