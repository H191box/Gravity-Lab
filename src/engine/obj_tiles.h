#ifndef OBJ_TILES_H
#define OBJ_TILES_H

#include "gba_types.h"

/* ============================================================
 *  OBJ GameSprite Tile IDs
 *
 *  GBA 1D sprite mapping: tile index N → VRAM 0x06000000 + N*32
 *  BG CharBase 0 occupies tile indices 0-511 (VRAM 0x06000000-0x06003FFF).
 *  We place all OBJ tile data starting at tile index 512 (VRAM 0x06004000).
 *
 *  16x16 sprites = 4 tiles each, 8x8 sprites = 1 tile each.
 * ============================================================ */

#define OBJ_TILE_BASE       512  /* First OBJ tile index */

/* Ship frames (16x16 each = 4 tiles) */
#define OBJ_SHIP_CENTER     (OBJ_TILE_BASE + 0)    /* Tiles 0-3 */
#define OBJ_SHIP_LEFT       (OBJ_TILE_BASE + 4)    /* Tiles 4-7 */
#define OBJ_SHIP_RIGHT      (OBJ_TILE_BASE + 8)    /* Tiles 8-11 */
#define OBJ_SHIP_THRUST     (OBJ_TILE_BASE + 12)   /* Tiles 12-15 */

/* Explosion fireball (16x16 = 4 tiles) */
#define OBJ_EXPLOSION       (OBJ_TILE_BASE + 16)   /* Tiles 16-19 */

/* Obstacle bar (16x16 = 4 tiles) */
#define OBJ_OBSTACLE_BAR    (OBJ_TILE_BASE + 20)   /* Tiles 20-23 */

/* Medals (16x16 each = 4 tiles) */
#define OBJ_MEDAL_GOLD      (OBJ_TILE_BASE + 24)   /* Tiles 24-27 */
#define OBJ_MEDAL_SILVER    (OBJ_TILE_BASE + 28)   /* Tiles 28-31 */
#define OBJ_MEDAL_BRONZE    (OBJ_TILE_BASE + 32)   /* Tiles 32-35 */

/* 8x8 particles (1 tile each) */
#define OBJ_PARTICLE_SPARK      (OBJ_TILE_BASE + 36)  /* Tile 36 */
#define OBJ_PARTICLE_GLOW       (OBJ_TILE_BASE + 37)  /* Tile 37 */
#define OBJ_PARTICLE_SMOKE      (OBJ_TILE_BASE + 38)  /* Tile 38 */
#define OBJ_PARTICLE_CHECKPOINT (OBJ_TILE_BASE + 39)  /* Tile 39 */

#endif /* OBJ_TILES_H */
