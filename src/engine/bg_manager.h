#ifndef BG_MANAGER_H
#define BG_MANAGER_H

#include "gba_types.h"

/* Background layer IDs */
#define BG_LAYER_FLOOR   0  /* BG0: Circuit floor tiles */
#define BG_LAYER_WALLS   1  /* BG1: Walls and obstacles overlay */
#define BG_LAYER_DECO    2  /* BG2: Decorative elements */
#define BG_LAYER_UI      3  /* BG3: UI background / text */

/* Background layer register pointers */
#define BG_REG_CNT(n)    (*(volatile u16 *)(0x04000008 + (n) * 2))
#define BG_REG_HOFS(n)   (*(volatile u16 *)(0x04000010 + (n) * 4))
#define BG_REG_VOFS(n)   (*(volatile u16 *)(0x04000012 + (n) * 4))

/* BG configuration structure */
typedef struct {
    u8 priority;      /* 0 (highest) to 3 (lowest) */
    u8 char_base;     /* Character base block (0-3, each 16KB) */
    u8 screen_base;   /* Screen base block (0-31, each 2KB) */
    u8 size;          /* 0=32x32, 1=64x64, 2=32x64, 3=64x32 */
    bool palette_256; /* TRUE = 256-color, FALSE = 16-color */
    bool wrap;        /* TRUE = wrap around edges */
    bool enabled;     /* TRUE = layer is active */
} BGConfig;

/* -------------------------------------------------------
 *  bg_init — Initialize all 4 background layers
 *  Sets up default configurations for each layer.
 * ------------------------------------------------------- */
void bg_init(void);

/* -------------------------------------------------------
 *  bg_configure — Configure a specific BG layer
 * ------------------------------------------------------- */
void bg_configure(int layer, const BGConfig *cfg);

/* -------------------------------------------------------
 *  bg_set_priority — Change a layer's display priority
 * ------------------------------------------------------- */
void bg_set_priority(int layer, u8 priority);

/* -------------------------------------------------------
 *  bg_scroll — Set scroll offset for a layer
 *  x, y: scroll position in pixels (0-511 for 512px maps)
 * ------------------------------------------------------- */
void bg_set_scroll(int layer, s16 x, s16 y);

/* -------------------------------------------------------
 *  bg_get_scroll — Get current scroll offset
 * ------------------------------------------------------- */
void bg_get_scroll(int layer, s16 *x, s16 *y);

/* -------------------------------------------------------
 *  bg_enable / bg_disable — Toggle layer visibility
 *  These modify REG_DISPCNT
 * ------------------------------------------------------- */
void bg_enable(int layer);
void bg_disable(int layer);

/* -------------------------------------------------------
 *  bg_set_map_entry — Write a tile entry to the BG map
 *  layer: BG layer (0-3)
 *  col, row: Tile position in the map
 *  tile_id: Tile index to place
 *  palette: Palette number (0-15 for 16-color)
 *  hflip, vflip: Tile flipping flags
 * ------------------------------------------------------- */
void bg_set_map_entry(int layer, u8 col, u8 row, u16 tile_id,
                       u8 palette, bool hflip, bool vflip);

/* -------------------------------------------------------
 *  bg_get_map_entry — Read a tile entry from the BG map
 *  Returns the raw 16-bit map entry
 * ------------------------------------------------------- */
u16 bg_get_map_entry(int layer, u8 col, u8 row);

/* -------------------------------------------------------
 *  bg_clear_map — Clear all tiles in a BG map to tile 0
 * ------------------------------------------------------- */
void bg_clear_map(int layer);

/* -------------------------------------------------------
 *  bg_get_map_address — Get the VRAM address of a BG's tile map
 * ------------------------------------------------------- */
u16 *bg_get_map_address(int layer);

/* -------------------------------------------------------
 *  bg_get_char_address — Get the VRAM address of a BG's character data
 * ------------------------------------------------------- */
u16 *bg_get_char_address(int layer);

#endif /* BG_MANAGER_H */